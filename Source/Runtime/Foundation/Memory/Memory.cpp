// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Memory.h"
#include "HeapAllocator.h"
#include "ProxyAllocator.h"

namespace sb
{

	namespace memory
	{
		static const int ALLOCATOR_SIZE = sizeof(HeapAllocator);
		char g_allocator_buffer[2 * ALLOCATOR_SIZE];

		HeapAllocator* g_default_alloc = nullptr;
#ifdef SANDBOX_MEMORY_TRACKING
		HeapAllocator* g_debug_alloc = nullptr;
#endif

		TraceAllocator* g_default_proxy_allocator = nullptr;
	};

	void memory::Initialize()
	{
		g_default_alloc = new (g_allocator_buffer)HeapAllocator();

#ifdef SANDBOX_MEMORY_TRACKING
		g_debug_alloc = new (g_allocator_buffer + ALLOCATOR_SIZE) HeapAllocator();
		g_default_proxy_allocator = SB_NEW(*g_debug_alloc, TraceAllocator, "Default", *g_default_alloc);
#endif

	}
	void memory::Shutdown()
	{
#ifdef SANDBOX_MEMORY_TRACKING
		if (g_default_proxy_allocator)
		{
			SB_DELETE(*g_debug_alloc, g_default_proxy_allocator);
		}
#endif

		// Because the allocator is stored on the stack and we can't use delete we have 
		//	to call the destructor ourself
		g_default_alloc->~HeapAllocator();
		g_default_alloc = nullptr;

#ifdef SANDBOX_MEMORY_TRACKING
		g_debug_alloc->~HeapAllocator();
		g_debug_alloc = nullptr;
#endif
	}
	Allocator& memory::DefaultAllocator()
	{
#ifdef SANDBOX_MEMORY_TRACKING
		return *g_default_proxy_allocator;
#else
		return *g_default_alloc;
#endif
	}
	Allocator& memory::ScratchAllocator()
	{
#ifdef SANDBOX_MEMORY_TRACKING
		return *g_default_proxy_allocator;
#else
		return *g_default_alloc; // TODO
#endif
	}

#ifdef SANDBOX_MEMORY_TRACKING
	Allocator& memory::DebugAllocator()
	{
		return *g_debug_alloc;
	}
#endif

	void* memory::Malloc(size_t size, uint32_t alignment)
	{
		Assert(g_default_alloc);
		return g_default_alloc->Allocate(size, alignment);
	}
	void* memory::Realloc(void* p, size_t size)
	{
		Assert(g_default_alloc);
		return g_default_alloc->Reallocate(p, size);
	}
	void memory::Free(void* p)
	{
		Assert(g_default_alloc);
		g_default_alloc->Free(p);
	}
	size_t memory::GetAllocatedSize(void* p)
	{
		Assert(g_default_alloc);
		return g_default_alloc->GetAllocatedSize(p);
	}

} // namespace sb
