// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "System.h"

///
/// @file System_Win.cpp
///	@brief Some platform dependent functions for the system
///

#include <dbghelp.h>

#pragma comment ( lib, "dbghelp.lib" )


namespace sb
{

	namespace system
	{
		CriticalSection g_stack_walk_lock;

		void SymInitialize();
	};
	void system::SymInitialize()
	{
		static bool is_sym_initialized = false;
		if (!is_sym_initialized)
		{
			BOOL res = ::SymInitialize(GetCurrentProcess(), NULL, true);
			if (res)
			{
				logging::Info("Symbol handler initialized");
			}
			else
			{
				logging::Info("Failed to initialize symbol handler (Error code: %u)", ::GetLastError());
			}

			is_sym_initialized = true;
		}
	}

	uint32_t system::GetStackTrace(void** addresses, uint32_t max_addresses, uint32_t skip_count)
	{
#ifdef SANDBOX_PLATFORM_WIN64
		Assert(false); // Not implemtented
#endif

		Assert(addresses);
		Assert(max_addresses != 0);

		ScopedLock<CriticalSection> scoped_lock(g_stack_walk_lock);
		system::SymInitialize();

		HANDLE process_handle = GetCurrentProcess();
		HANDLE thread_handle = GetCurrentThread();

		// Capture current context
		CONTEXT context;
		RtlCaptureContext(&context);

		// Intialize our stack frame struct
		STACKFRAME64 frame;
		ZeroMemory(&frame, sizeof(STACKFRAME64));

		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrFrame.Mode = AddrModeFlat;
		frame.AddrStack.Mode = AddrModeFlat;

#ifdef SANDBOX_PLATFORM_WIN64
		frame.AddrPC.Offset = context.Rip;
		frame.AddrFrame.Offset = context.Rbp;
		frame.AddrStack.Offset = context.Rsp;
#else
		frame.AddrPC.Offset = context.Eip;
		frame.AddrFrame.Offset = context.Ebp;
		frame.AddrStack.Offset = context.Esp;
#endif

		// Skip the call here
		skip_count++;
		// Just walk through and skip storing our first skipped frames
		for (uint32_t i = 0; i < skip_count; ++i)
		{
			BOOL res = StackWalk64(IMAGE_FILE_MACHINE_I386, process_handle, thread_handle, &frame, &context, NULL,
				SymFunctionTableAccess64, SymGetModuleBase64, NULL);
			if (!res || frame.AddrPC.Offset == 0) // Is this the end of our walk?
			{
				return 0;
			}
		}

		for (uint32_t i = 0; i < max_addresses; ++i)
		{
			BOOL res = StackWalk64(IMAGE_FILE_MACHINE_I386, process_handle, thread_handle, &frame, &context, NULL,
				SymFunctionTableAccess64, SymGetModuleBase64, NULL);

			if (!res || frame.AddrPC.Offset == 0) // Is this the end of our walk?
			{
				return i;
			}
			addresses[i] = (void*)frame.AddrPC.Offset;
		}
		return max_addresses;
	}
	void system::GetAddressSymbol(string& symbol, void* address)
	{
		Assert(address);
		ScopedLock<CriticalSection> scoped_lock(g_stack_walk_lock);
		system::SymInitialize();

		HANDLE process_handle = GetCurrentProcess();

		char buffer[sizeof(SYMBOL_INFO)+MAX_SYM_NAME * sizeof(TCHAR)];
		PSYMBOL_INFO symbol_info = (PSYMBOL_INFO)buffer;

		symbol_info->MaxNameLen = MAX_SYM_NAME;
		symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
		if (SymFromAddr(process_handle, (DWORD64)address, NULL, symbol_info))
		{
			symbol_info->Name[MAX_SYM_NAME - 1] = '\0';
			symbol += symbol_info->Name; symbol += " ";
		}
		else
		{
			symbol += "(Undefined) ";
		}

		DWORD displacement = 0;
		IMAGEHLP_LINE64 line_info;
		if (SymGetLineFromAddr64(process_handle, (DWORD64)address, &displacement, &line_info))
		{
			char line_number[8];
			sprintf(line_number, "%u", line_info.LineNumber);
			line_number[7] = '\0';
			symbol += "(";
			symbol += line_info.FileName;
			symbol += ":";
			symbol += line_number;
			symbol += ")";
		}
		else
		{
			symbol += "(Undefined)";
		}
	}


	//-------------------------------------------------------------------------------
	void system::GetSystemInfo(SystemInfo& info)
	{
		SYSTEM_INFO si;
		::GetSystemInfo(&si);

		info.num_processors = si.dwNumberOfProcessors;
		info.page_size = si.dwPageSize;
	}
	//-------------------------------------------------------------------------------

} // namespace sb

