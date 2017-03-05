// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_LINEARALLOCATORWITHBUFFER_H__
#define __FOUNDATION_LINEARALLOCATORWITHBUFFER_H__


namespace sb
{

	/// @brief A scope local allocator for temporary allocations
	///
	/// This allocator primarly allocates memory from the local stack.
	///	If that memory is exhausted it uses the specified backing allocator.
	/// This allocator cannot free specific allocations, it will all be freed
	///	when the allocator is destroyed. 
	///	@tparam BUFFER_SIZE Size of the local stack buffer in bytes.
	template <int BUFFER_SIZE>
	class LinearAllocatorWithBuffer
	{
		Allocator& _backing;
		uint8_t _buffer[BUFFER_SIZE];
		uint8_t* _start;
		uint8_t* _pos; ///< Current position in buffer
		uint8_t* _end;

		uint32_t _chunk_size; ///< Chunk size for allocation from backing allocator

	public:
		LinearAllocatorWithBuffer(Allocator& backing = memory::DefaultAllocator());
		~LinearAllocatorWithBuffer();

		void* Allocate(uint32_t size, uint32_t alignment = memory::DEFAULT_ALIGNMENT);
		void* Reallocate(void* p, uint32_t size);

		/// This allocator cannot free specific memory so this method is empty 
		void Free(void*) { }

		/// Returns 0 as this allocator doesn't track memory 
		uint32_t GetAllocatedSize(void*) { return 0; }
	};

	//-------------------------------------------------------------------------------
	template <int BUFFER_SIZE>
	LinearAllocatorWithBuffer<BUFFER_SIZE>::LinearAllocatorWithBuffer(Allocator& backing) : _backing(backing)
	{
		_chunk_size = 4 * 1024;
		_pos = _start = _buffer;
		_end = _start + BUFFER_SIZE;
		*(void**)_start = 0;	// We will store pointers to the next memory region allocated
		//	in the top of each chunk, this is so we can free them later
		_pos += sizeof(void*);
	}

	template <int BUFFER_SIZE>
	LinearAllocatorWithBuffer<BUFFER_SIZE>::~LinearAllocatorWithBuffer()
	{
		// Free any memory we have allocated with our backing allocator
		void* p = *(void**)_buffer;
		while (p)
		{
			void* next = *(void**)p;
			_backing.Free(p);
			p = next;
		}
	}

	template <int BUFFER_SIZE>
	void* LinearAllocatorWithBuffer<BUFFER_SIZE>::Allocate(uint32_t size, uint32_t alignment)
	{
		_pos = (uint8_t*)memory::AlignForward(_pos, (size_t)alignment);
		// Check if this allocation fits in the current region
		if ((int)size > _end - _pos)
		{
			// If not: Allocate a new chunk from our backing allocator
			uint32_t to_allocate = sizeof(void*)+size + alignment; // Bytes to allocate
			if (to_allocate < _chunk_size)
				to_allocate = _chunk_size;
			// Double the size each time we allocate an chunk, this way we minimize the number of allocations
			_chunk_size *= 2;
			void* p = _backing.Allocate(to_allocate);
			*(void**)_start = p;
			_pos = _start = (uint8_t*)p;
			_end = _start + to_allocate;
			*(void**)_start = 0;
			_pos += sizeof(void*);
			_pos = (uint8_t*)memory::AlignForward(_pos, (size_t)alignment);

#ifdef SANDBOX_MEMORY_TRACKING
			logging::Info("MemoryTracker: LinearAllocatorWithBuffer<%d> allocating additional memory (New size: %d).", BUFFER_SIZE, (_end - _pos));
#endif
		}
		void* res = _pos;
		_pos += size;
		return res;
	}

	template <int BUFFER_SIZE>
	void* LinearAllocatorWithBuffer<BUFFER_SIZE>::Reallocate(void*, uint32_t size)
	{
		return Allocate(size);
	}

} // namespace sb

#endif // __FOUNDATION_LINEARALLOCATORWITHBUFFER_H__

