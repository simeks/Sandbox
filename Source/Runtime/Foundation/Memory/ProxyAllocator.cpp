// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ProxyAllocator.h"
#include "Platform/System.h"

namespace sb
{

#ifdef SANDBOX_MEMORY_TRACKING
	//-------------------------------------------------------------------------------
	BasicMemoryTracker::BasicMemoryTracker(ProxyAllocator<BasicMemoryTracker>& owner)
		: _allocator(owner)
	{
		_allocation_count = 0;
		_bytes_allocated = 0;
	}
	BasicMemoryTracker::~BasicMemoryTracker()
	{
		if (_allocation_count != 0 || _bytes_allocated != 0)
		{
			logging::Warning("Memory leak detected! (Allocator: %s, Allocations: %u, Bytes: %u)",
				_allocator.GetName(), _allocation_count, _bytes_allocated);
		}

		AssertMsg((_allocation_count == 0 || _bytes_allocated == 0), "Memory leak detected!");
	}
	void BasicMemoryTracker::OnAllocate(void* p)
	{
		size_t bytes = _allocator.GetAllocatedSize(p);
		if (bytes == 0)
			return;	// Nothing allocated, most likely using an allocator not tracking size

#ifdef SANDBOX_PLATFORM_WIN64
		thread::InterlockedIncrement64((int64_t*)&_allocation_count);
		thread::InterlockedExchangeAdd64((int64_t*)&_bytes_allocated, bytes);
#else
		thread::InterlockedIncrement((long*)&_allocation_count);
		thread::InterlockedExchangeAdd((long*)&_bytes_allocated, bytes);
#endif
	}
	void BasicMemoryTracker::OnFree(void* p)
	{
		if (!p) // Ignore NULL pointers
			return;

		size_t bytes = _allocator.GetAllocatedSize(p);
		if (bytes == 0)
			return;	// Nothing allocated, most likely using an allocator not tracking size
#ifdef SANDBOX_PLATFORM_WIN64
		thread::InterlockedDecrement64((int64_t*)&_allocation_count);
		thread::_InterlockedExchangeAdd64((int64_t*)&_bytes_allocated, -(int64_t)bytes);
#else
		thread::InterlockedDecrement((long*)&_allocation_count);
		thread::InterlockedExchangeSubtract((unsigned long*)&_bytes_allocated, bytes);
#endif
	}
	//-------------------------------------------------------------------------------

	bool VerboseMemoryTracker::s_disable_stack_trace = false;

	VerboseMemoryTracker::VerboseMemoryTracker(ProxyAllocator<VerboseMemoryTracker>& owner)
		: _allocator(owner)
	{
		_allocation_count = 0;
		_bytes_allocated = 0;
	}
	VerboseMemoryTracker::~VerboseMemoryTracker()
	{
		if (_allocation_count != 0 || _bytes_allocated != 0)
		{
			logging::Warning("Memory leak detected!");
			PrintMemoryStats();
		}

		AssertMsg((_allocation_count == 0 || _bytes_allocated == 0), "Memory leak detected!");
	}
	void VerboseMemoryTracker::OnAllocate(void* p)
	{
		size_t bytes = _allocator.GetAllocatedSize(p);
		if (bytes == 0)
			return;	// Nothing allocated, most likely using an allocator not tracking size

#ifdef SANDBOX_PLATFORM_WIN64
		thread::InterlockedIncrement64((int64_t*)&_allocation_count);
		thread::InterlockedExchangeAdd64((int64_t*)&_bytes_allocated, bytes);
#else
		thread::InterlockedIncrement((long*)&_allocation_count);
		thread::InterlockedExchangeAdd((long*)&_bytes_allocated, bytes);
#endif

		// We have to mark the stack trace as disabled when doing a stack trace ourself,
		//	because there's a chance for recursion if (in this case) std::map decides to
		//	allocate more memory when we're storing our stack trace. This would otherwise
		//	cause deadlocks.
		if (!s_disable_stack_trace)
		{
			ScopedLock<CriticalSection> scoped_lock(_allocation_stack_traces_lock);
			s_disable_stack_trace = true;

			AllocationStackTrace stack_trace;
			memset(&stack_trace, 0, sizeof(AllocationStackTrace));
			system::GetStackTrace(stack_trace.addresses, MAX_STACKTRACE_DEPTH, 1);
			_allocation_stack_traces[p] = stack_trace;

			s_disable_stack_trace = false;
		}
	}
	void VerboseMemoryTracker::OnFree(void* p)
	{
		if (!p) // Ignore NULL pointers
			return;

		size_t bytes = _allocator.GetAllocatedSize(p);
		if (bytes == 0)
			return;	// Nothing allocated, most likely using an allocator not tracking size

#ifdef SANDBOX_PLATFORM_WIN64
		thread::InterlockedDecrement64((int64_t*)&_allocation_count);
		thread::_InterlockedExchangeAdd64((int64_t*)&_bytes_allocated, -(int64_t)bytes);
#else
		thread::InterlockedDecrement((long*)&_allocation_count);
		thread::InterlockedExchangeSubtract((unsigned long*)&_bytes_allocated, bytes);
#endif

		if (!s_disable_stack_trace)
		{
			ScopedLock<CriticalSection> scoped_lock(_allocation_stack_traces_lock);
			s_disable_stack_trace = true;

			if (_allocation_stack_traces.find(p) != _allocation_stack_traces.end())
				_allocation_stack_traces.erase(p);

			s_disable_stack_trace = false;
		}
	}
	void VerboseMemoryTracker::PrintMemoryStats()
	{
		const char* name = _allocator.GetName();
		if (!name)
			name = "Unnamed";

		logging::Info("Allocator: %s, Allocations: %d, Bytes: %d", name, _allocation_count, _bytes_allocated);
		logging::Info("Unfreed allocations:");

		if (!s_disable_stack_trace)
		{
			ScopedLock<CriticalSection> scoped_lock(_allocation_stack_traces_lock);
			s_disable_stack_trace = true;

			string symbol;

			StackTraceMap::iterator it, end;
			end = _allocation_stack_traces.end();
			for (it = _allocation_stack_traces.begin(); it != end; ++it)
			{
				logging::Info("Allocation: 0x%p (Size: %u):", it->first, _allocator.GetAllocatedSize(it->first));
				for (uint32_t i = 0; i < MAX_STACKTRACE_DEPTH; ++i)
				{
					void* addr = it->second.addresses[i];
					if (addr != nullptr)
					{
						symbol.clear();
						system::GetAddressSymbol(symbol, addr);
						logging::Info("\t%s", symbol.c_str());
					}
				}
			}

			s_disable_stack_trace = false;
		}
	}



#endif // #ifdef SANDBOX_MEMORY_TRACKING


} // namespace sb
