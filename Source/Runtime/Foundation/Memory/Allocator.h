// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_ALLOCATOR_H__
#define __FOUNDATION_ALLOCATOR_H__


namespace sb
{

	/// @brief Interface for allocators
	class Allocator
	{
	public:
		virtual ~Allocator() {}

		/// @brief Allocated the specified amount of memory with the specified alignment
		virtual void* Allocate(size_t size, uint32_t alignment = memory::DEFAULT_ALIGNMENT) = 0;

		/// @brief Changes the size of the specified memory block
		virtual void* Reallocate(void* p, size_t size) = 0;

		/// @brief Frees memory previously allocated with Allocate()
		virtual void Free(void* p) = 0;

		/// @brief Returns the amount of memory allocted at p
		///
		///	@param p Must be a pointer to memory allocated with Allocate()
		virtual size_t GetAllocatedSize(void* p) = 0;


	};

} // namespace sb

#endif // __FOUNDATION_ALLOCATOR_H__

