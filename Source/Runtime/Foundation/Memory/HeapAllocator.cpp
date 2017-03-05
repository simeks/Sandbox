// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "HeapAllocator.h"

#include "Platform/System.h"
#include "Thread/Thread.h"

namespace sb
{

	// Doug Lea's Malloc (http://g.oswego.edu/dl/html/malloc.html)

	// MSPACES                  default: 0 (false)
	//  If true, compile in support for independent allocation spaces.
	//  This is only supported if HAVE_MMAP is true.
#define MSPACES 1

	// ONLY_MSPACES             default: 0 (false)
	//  If true, only compile in mspace versions, not regular versions.
#define ONLY_MSPACES 1

	// USE_DL_PREFIX            default: NOT defined
	//  Causes compiler to prefix all public routines with the string 'dl'.
	//  This can be useful when you only want to use this malloc in one part
	//  of a program, using your regular system malloc elsewhere.
#define USE_DL_PREFIX 1

#define CORRUPTION_ERROR_ACTION( m ) AssertMsg( false, "dlmalloc: Corruption error!" );
#define USAGE_ERROR_ACTION( m, p ) AssertMsg( false, "dlmalloc: Usage error!" );


	// INSECURE                 default: 0
	//  If true, omit checks for usage errors and heap space overwrites.
#if defined(SANDBOX_BUILD_RELEASE) || defined(SANDBOX_PROFILE)
#define INSECURE 1
#else
#define INSECURE 0
#endif

#define USE_LOCKS 2
#define MLOCK_T MallocLock

	struct MallocLock
	{
		// Buffer for our CriticalSection object
		uint8_t buffer[sizeof(CriticalSection)];
	};
	static int MallocLockInitial(MLOCK_T* lk)
	{
		new (lk->buffer) CriticalSection();
		return 0;
	}
	static int MallocLockDestroy(MLOCK_T* lk)
	{
		CriticalSection* cs = reinterpret_cast<CriticalSection*>(lk);
		cs->~CriticalSection();

		return 0;
	}
	static int MallocLockAcquire(MLOCK_T* lk)
	{
		CriticalSection* cs = reinterpret_cast<CriticalSection*>(lk);
		cs->Lock();
		return 0;
	}
	static int MallocLockRelease(MLOCK_T* lk)
	{
		CriticalSection* cs = reinterpret_cast<CriticalSection*>(lk);
		cs->Unlock();
		return 0;
	}
	//static int MallocLockTry(MLOCK_T* lk)
	//{
	//	CriticalSection* cs = reinterpret_cast<CriticalSection*>(lk);
	//	return (int)cs->TryLock();
	//}

#define INITIAL_LOCK(lk) MallocLockInitial(lk)
#define DESTROY_LOCK(lk) MallocLockDestroy(lk)
#define ACQUIRE_LOCK(lk) MallocLockAcquire(lk)
#define RELEASE_LOCK(lk) MallocLockRelease(lk)
	// #define TRY_LOCK(lk) MallocLockTry(lk)

	static CriticalSection g_malloc_global_lock;

#define ACQUIRE_MALLOC_GLOBAL_LOCK() g_malloc_global_lock.Lock()
#define RELEASE_MALLOC_GLOBAL_LOCK() g_malloc_global_lock.Unlock()

#pragma warning( push )
#pragma warning( disable : 6285 )
#pragma warning( disable : 6326 )
#pragma warning( disable : 28159 )
#pragma warning( disable : 28182 )
#pragma warning( disable : 6326 )

#include "dlmalloc.c"

#pragma warning( pop ) 


	//-------------------------------------------------------------------------------
	HeapAllocator::HeapAllocator()
	{
		_mspace = create_mspace(0, 1);
	}
	HeapAllocator::~HeapAllocator()
	{
		destroy_mspace(_mspace);
	}
	//-------------------------------------------------------------------------------
	void* HeapAllocator::Allocate(size_t size, uint32_t alignment)
	{
		Assert(size != 0);

		void* p = nullptr;
		if (alignment <= memory::DEFAULT_ALIGNMENT)
		{
			p = mspace_malloc(_mspace, size);
		}
		else
		{
			p = mspace_memalign(_mspace, alignment, size);
		}
		Assert(p);

		return p;
	}
	void* HeapAllocator::Reallocate(void* p, size_t size)
	{
		Assert(size != 0);
		p = mspace_realloc(_mspace, p, size);

		return p;
	}
	void HeapAllocator::Free(void* p)
	{
		if (p == nullptr)
			return;

		mspace_free(_mspace, p);
	}
	size_t HeapAllocator::GetAllocatedSize(void* p)
	{
		return mspace_usable_size(p);
	}
	//-------------------------------------------------------------------------------

} // namespace sb
