// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "LinearAllocator.h"


namespace sb
{

	LinearAllocator::LinearAllocator(size_t chunk_size, Allocator& backing)
		: _backing(backing),
		_default_chunk_size(chunk_size),
		_first_chunk(nullptr),
		_current_chunk(nullptr),
		_chunk_position(nullptr),
		_remaining_bytes(0)
	{
		AllocateChunk(_default_chunk_size);
	}
	LinearAllocator::~LinearAllocator()
	{
		ChunkHeader* chunk = (ChunkHeader*)_first_chunk;
		while (chunk)
		{
			ChunkHeader* next = (ChunkHeader*)chunk->next_chunk;
			_backing.Free(chunk);

			chunk = next;
		}
	}

	void* LinearAllocator::Allocate(size_t size, uint32_t alignment)
	{
		// Do we need to allocate a new chunk?
		if ((size + alignment) > _remaining_bytes)
			AllocateChunk(size + alignment);

		void* p = memory::AlignForward(_chunk_position, (size_t)alignment);

		Assert(p >= _chunk_position);
		_remaining_bytes -= (((uint8_t*)(p)-(uint8_t*)(_chunk_position)) + size);
		_chunk_position = memory::PointerAdd(p, size);

		return p;
	}
	void* LinearAllocator::Reallocate(void*, size_t)
	{
		AssertMsg(false, "Not supported.");
		return nullptr;
	}

	void LinearAllocator::Free(void*)
	{
	}

	void LinearAllocator::Reset()
	{
		_current_chunk = _first_chunk;
		_remaining_bytes = ((ChunkHeader*)_current_chunk)->chunk_size;
		_chunk_position = memory::PointerAdd(_current_chunk, sizeof(ChunkHeader));
	}
	size_t LinearAllocator::GetAllocatedSize(void*)
	{
		AssertMsg(false, "Not supported.");
		return 0;
	}

	void LinearAllocator::AllocateChunk(size_t min_size)
	{
		size_t chunk_size = Max(min_size, _default_chunk_size);

		// First check if there's any chunks after the current one
		ChunkHeader* current_chunk_header = (ChunkHeader*)_current_chunk;
		if (current_chunk_header && current_chunk_header->next_chunk)
		{
			// If that's the case, use that one if it's big enough
			ChunkHeader* next_chunk = (ChunkHeader*)current_chunk_header->next_chunk;
			if (next_chunk->chunk_size < chunk_size)
			{
				// Create a new chunk big enough and link between the current one and next_chunk
				ChunkHeader* new_chunk = (ChunkHeader*)_backing.Allocate(chunk_size + sizeof(ChunkHeader));
				new_chunk->chunk_size = chunk_size;

				current_chunk_header->next_chunk = new_chunk;
				new_chunk->next_chunk = next_chunk;

				_current_chunk = new_chunk;
			}
			else
			{
				// Otherwise we just use the next one.
				_current_chunk = next_chunk;
			}
		}
		else
		{
			// If not, create a new one
			ChunkHeader* new_chunk = (ChunkHeader*)_backing.Allocate(chunk_size + sizeof(ChunkHeader));
			new_chunk->next_chunk = nullptr;
			new_chunk->chunk_size = chunk_size;

			// Link the new chunk to the current one.
			if (_current_chunk)
			{
				current_chunk_header->next_chunk = new_chunk;
			}
			else
			{
				// This is our first chunk
				_first_chunk = new_chunk;
			}
			_current_chunk = (void*)new_chunk;
		}

		_chunk_position = memory::PointerAdd(_current_chunk, sizeof(ChunkHeader));
		_remaining_bytes = ((ChunkHeader*)_current_chunk)->chunk_size;
	}

} // namespace sb

