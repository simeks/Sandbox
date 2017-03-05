// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_LINEARALLOCATOR_H__
#define __FOUNDATION_LINEARALLOCATOR_H__


namespace sb
{

	/// @brief A simple non-thread-safe allocator performing linear allocations.
	///	This allocator supports releasing all memory in one go, but it doesn't support releasing individual blocks.
	///		Therefore this allocator is very fitting when you know that you can release all memory at the same time.
	class LinearAllocator
	{
	public:
		/// Constructor
		/// @param allocator The backing allocator.
		/// @param chunk_size Size of each chunk allocated.
		LinearAllocator(size_t chunk_size, Allocator& backing = memory::DefaultAllocator());
		~LinearAllocator();

		void* Allocate(size_t size, uint32_t alignment = memory::DEFAULT_ALIGNMENT);
		void* Reallocate(void* p, size_t size);

		/// @remark This will not have any effect on this allocator.
		void Free(void* p);

		/// Resets the allocator, basically releasing all allocations.
		void Reset();

		/// @remark This is not supported by this allocator and will therefore cause an assertion.
		size_t GetAllocatedSize(void* p);

	private:
		/// @param min_size Minimum size of the new chunk, in case we need chunks larger than the default size.
		void AllocateChunk(size_t min_size);

	private:
		/// @brief This object will be on the top of each chunk.
		struct ChunkHeader
		{
			void* next_chunk; // Pointer to the next chunk.
			size_t chunk_size; // Size of this chunk. Does not include the size of the header.
		};

		Allocator& _backing;
		size_t _default_chunk_size;

		void* _first_chunk;
		void* _current_chunk;

		void* _chunk_position; // Position in the current chunk
		size_t _remaining_bytes; // Remaining bytes in the current chunk


	};

} // namespace sb

#endif // __FOUNDATION_LINEARALLOCATOR_H__
