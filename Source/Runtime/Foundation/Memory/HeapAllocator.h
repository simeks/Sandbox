// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_HEAPALLOCATOR_H__
#define __FOUNDATION_HEAPALLOCATOR_H__

#include "Memory.h"
#include "Allocator.h"

namespace sb
{

	///	Uses dlmalloc for allocations.
	class HeapAllocator : public Allocator
	{
	public:
		HeapAllocator();
		~HeapAllocator();

		void* Allocate(size_t size, uint32_t alignment = memory::DEFAULT_ALIGNMENT);
		void* Reallocate(void* p, size_t size);
		void Free(void* p);

		size_t GetAllocatedSize(void* p);

	private:
		void* _mspace;

	};

} // namespace sb



#endif // __FOUNDATION_HEAPALLOCATOR_H__

