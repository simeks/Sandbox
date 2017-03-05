/// Copyright 2008-2014 Simon Ekström

#ifndef __CORE_PROXYALLOCATOR_H__
#define __CORE_PROXYALLOCATOR_H__

#include "Memory.h"
#include "StlAllocator.h"

#include <Foundation/Thread/Thread.h>


namespace sb
{

	class BasicMemoryTracker;

	//-------------------------------------------------------------------------------

	/// @brief Proxy allocator used for debugging
	///
	/// An allocator working as a proxy for an actual allocator, for debugging
	///	purposes it tracks all allocations going through it.
	template<class MemoryTracker>
	class ProxyAllocator : public Allocator
	{
		Allocator&	_allocator;
		const char* _name;

		MemoryTracker _memory_tracker;

	public:
		ProxyAllocator(const char* name, Allocator& backing);
		~ProxyAllocator();

		void* Allocate(size_t size, uint32_t alignment = DEFAULT_ALIGN);
		void* Reallocate(void* p, size_t size);
		void Free(void* p);

		size_t GetAllocatedSize(void* p);

		const char* GetName() const;

		MemoryTracker& Tracker()  { return _memory_tracker; }
	};

	//-------------------------------------------------------------------------------

	class NoTracking
	{
	public:
		NoTracking(ProxyAllocator<NoTracking>&) {}
		~NoTracking() {}

		INLINE void OnAllocate(void*) {}
		INLINE void OnFree(void*) {}
	};

#ifdef SANDBOX_MEMORY_TRACKING
	class BasicMemoryTracker
	{
		ProxyAllocator<BasicMemoryTracker>&	_allocator;

		volatile size_t _allocation_count;
		volatile size_t _bytes_allocated;

		const BasicMemoryTracker& operator=(const BasicMemoryTracker&) { return *this; }
	public:
		BasicMemoryTracker(ProxyAllocator<BasicMemoryTracker>& owner);
		~BasicMemoryTracker();

		void OnAllocate(void* p);
		void OnFree(void* p);
	};

	class VerboseMemoryTracker
	{
	public:
		static const uint32_t MAX_STACKTRACE_DEPTH = 32;

		struct AllocationStackTrace
		{
			void* addresses[MAX_STACKTRACE_DEPTH];
		};

	public:
		ProxyAllocator<VerboseMemoryTracker>& _allocator;

		// TODO: Change to alternative for std::hash_map
		typedef std::unordered_map<void*, AllocationStackTrace, std::hash<void*>, std::equal_to<void*>, stl_debug_allocator<pair<void*, AllocationStackTrace>>> StackTraceMap;
		StackTraceMap _allocation_stack_traces;
		CriticalSection _allocation_stack_traces_lock;
		static bool s_disable_stack_trace;

		volatile uint32_t _allocation_count;
		volatile uint32_t _bytes_allocated;

		const VerboseMemoryTracker& operator=(const VerboseMemoryTracker&) { return *this; }
	public:
		VerboseMemoryTracker(ProxyAllocator<VerboseMemoryTracker>& owner);
		~VerboseMemoryTracker();

		void OnAllocate(void* p);
		void OnFree(void* p);

		/// @brief Prints memory stats about this proxy
		void PrintMemoryStats();
	};
	typedef ProxyAllocator<VerboseMemoryTracker> VerboseTraceAllocator;


	typedef ProxyAllocator<BasicMemoryTracker> TraceAllocator;

#else // SANDBOX_MEMORY_TRACKING

	typedef ProxyAllocator<NoTracking> TraceAllocator;
	typedef ProxyAllocator<NoTracking> RemoteTraceAllocator;


#endif // SANDBOX_MEMORY_TRACKING

	//-------------------------------------------------------------------------------

	template<class MemoryTracker>
	ProxyAllocator<MemoryTracker>::ProxyAllocator(const char* name, Allocator& backing)
		: _allocator(backing), _name(name), _memory_tracker(*this)
	{
	}

	template<class MemoryTracker>
	ProxyAllocator<MemoryTracker>::~ProxyAllocator()
	{
	}

	template<class MemoryTracker>
	void* ProxyAllocator<MemoryTracker>::Allocate(size_t size, uint32_t alignment)
	{
		void* p = _allocator.Allocate(size, alignment);
		_memory_tracker.OnAllocate(p);
		return p;
	}
	template<class MemoryTracker>
	void* ProxyAllocator<MemoryTracker>::Reallocate(void* p, size_t size)
	{
		_memory_tracker.OnFree(p);
		p = _allocator.Reallocate(p, size);
		_memory_tracker.OnAllocate(p);
		return p;
	}
	template<class MemoryTracker>
	void ProxyAllocator<MemoryTracker>::Free(void* p)
	{
		if (p == nullptr)
			return;

		_memory_tracker.OnFree(p);
		_allocator.Free(p);
	}

	template<class MemoryTracker>
	size_t ProxyAllocator<MemoryTracker>::GetAllocatedSize(void* p)
	{
		return _allocator.GetAllocatedSize(p);
	}
	template<class MemoryTracker>
	const char* ProxyAllocator<MemoryTracker>::GetName() const
	{
		return _name;
	}

} // namespace sb


#endif // __CORE_PROXYALLOCATOR_H__
