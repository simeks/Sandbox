// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Logging.h"
#include "Log.h"

#include "Thread/Thread.h"

// Define to write log output to OutputDebugString
#ifdef SANDBOX_BUILD_DEBUG
#ifdef SANDBOX_PLATFORM_WIN
#define OUTPUTDEBUGSTRING(msg) OutputDebugString(msg)
#elif SANDBOX_PLATFORM_MACOSX
#define OUTPUTDEBUGSTRING(msg) printf("%s", msg)
#endif
#else
#define OUTPUTDEBUGSTRING(msg) ((void)0)
#endif

namespace sb
{

	//-------------------------------------------------------------------------------
	namespace
	{
		Log						g_main_log;
		CriticalSection			g_log_lock;

		logging::LogCallback	g_callback = 0;
		void*					g_callback_data = 0;
	}

	//-------------------------------------------------------------------------------
	void logging::Initialize(const char* file_name)
	{
		g_main_log.Open(file_name);
	}
	void logging::Shutdown()
	{
		g_main_log.Close();
	}
	void logging::SetOutputFile(const char* file_name)
	{
		ScopedLock<CriticalSection> scoped_lock(g_log_lock);
		if (g_main_log.IsOpen())
			g_main_log.Close();
		g_main_log.Open(file_name);
	}
	//-------------------------------------------------------------------------------
	void logging::Info(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		InfoV(fmt, args);
		va_end(args);
	}
	void logging::InfoV(const char* fmt, va_list arg)
	{
		ScopedLock<CriticalSection> scoped_lock(g_log_lock);

		char msg[MAX_MSG_SIZE];
		int size = vsnprintf(msg, MAX_MSG_SIZE - 2, fmt, arg);

		// Run callback before appending '\n'
		if (g_callback)
		{
			g_callback(g_callback_data, Log::LOG_INFO, msg);
		}

		if (size < 0) // Was string truncated?
		{
			msg[sizeof(msg)-2] = '\n'; msg[sizeof(msg)-1] = '\0';
		}
		else
		{
			strcat(msg, "\n");
		}

		if (g_main_log.IsOpen())
		{
			g_main_log.Write(Log::LOG_INFO, msg);
		}


		OUTPUTDEBUGSTRING(msg);
	}
	void logging::Warning(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		WarningV(fmt, args);
		va_end(args);
	}
	void logging::WarningV(const char* fmt, va_list arg)
	{
		ScopedLock<CriticalSection> scoped_lock(g_log_lock);

		char msg[MAX_MSG_SIZE];
		msg[0] = '\0';
		size_t len = 0;

		strcpy_s(msg, "[Warning] ");
		len = strlen(msg);

		int size = vsnprintf(msg + len, MAX_MSG_SIZE - len - 2, fmt, arg);

		// Run callback before appending '\n'
		if (g_callback)
		{
			g_callback(g_callback_data, Log::LOG_WARNING, msg + len);
		}


		if (size < 0) // Was string truncated?
		{
			msg[sizeof(msg)-2] = '\n'; msg[sizeof(msg)-1] = '\0';
		}
		else
		{
			strcat(msg, "\n");
		}
		if (g_main_log.IsOpen())
		{
			g_main_log.Write(Log::LOG_WARNING, msg + len);
		}

		OUTPUTDEBUGSTRING(msg);
	}
	void logging::Error(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		ErrorV(fmt, args);
		//va_end(args); // ErrorV will exit the program so the program will not even reach here.
	}
	void logging::ErrorV(const char* fmt, va_list arg)
	{
		{
			ScopedLock<CriticalSection> scoped_lock(g_log_lock);

			char msg[MAX_MSG_SIZE];
			size_t len = 0;

			strcpy_s(msg, "[Error] ");
			len = strlen(msg);

			int size = vsnprintf(msg + len, MAX_MSG_SIZE - len - 2, fmt, arg);

			// Run callback before appending '\n'
			if (g_callback)
			{
				g_callback(g_callback_data, Log::LOG_ERROR, msg + len);
			}
			if (size < 0) // Was string truncated?
			{
				msg[sizeof(msg)-2] = '\n'; msg[sizeof(msg)-1] = '\0';
			}
			else
			{
				strcat(msg, "\n");
			}
			if (g_main_log.IsOpen())
			{
				g_main_log.Write(Log::LOG_ERROR, msg);
			}


			OUTPUTDEBUGSTRING(msg);
		}

		exit(1);
	}

	void logging::AssertMessage(const char* fmt, ...)
	{
		ScopedLock<CriticalSection> scoped_lock(g_log_lock);

		char tmp[MAX_MSG_SIZE];
		size_t len = 0;

		strcpy_s(tmp, "[Assertion] ");
		len = strlen(tmp);

		va_list args;
		va_start(args, fmt);
		vsnprintf(tmp + len, MAX_MSG_SIZE - len - 2, fmt, args);
		va_end(args);

		tmp[sizeof(tmp)-2] = '\n'; tmp[sizeof(tmp)-1] = '\0';

		// Run callback before appending '\n'
		if (g_callback)
		{
			g_callback(g_callback_data, Log::LOG_ERROR, tmp + 12);
		}

		strcat_s(tmp, "\n");
		if (g_main_log.IsOpen())
		{
			g_main_log.Write(Log::LOG_ERROR, tmp);
		}


		OUTPUTDEBUGSTRING(tmp);

	}

	void logging::SetCallback(LogCallback callback, void* data)
	{
		g_callback = callback;
		g_callback_data = data;
	}

	//-------------------------------------------------------------------------------

} // namespace sb


