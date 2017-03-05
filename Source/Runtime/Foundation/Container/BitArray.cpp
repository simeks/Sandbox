// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "BitArray.h"


namespace sb
{

	//-------------------------------------------------------------------------------
	BitArray::BitArray(Allocator& allocator)
		: _allocator(&allocator), _buffer(0), _size(0), _capacity(0)
	{
	}
	BitArray::BitArray(const BitArray& other) : _allocator(other._allocator), _buffer(0)
	{
		// Calculate capacity, align to the number of bits in uint32_t
		_capacity = _size = other._size;

		size_t align = sizeof(size_t) * 8;
		size_t mod = _capacity % align;
		if (mod != 0)
			_capacity += (align - mod);

		if (_capacity != 0)
		{
			size_t bytes = _capacity / 8; // Bytes to allocate
			_buffer = (size_t*)_allocator->Allocate(bytes);
			memcpy(_buffer, other._buffer, bytes);
		}
	}
	BitArray::~BitArray()
	{
		_allocator->Free(_buffer);
		_buffer = 0;
		_size = 0;
	}
	//-------------------------------------------------------------------------------
	void BitArray::Fill(bool value)
	{
		if (_size)
		{
			size_t bytes = (_size + 7) / 8;
			memset(_buffer, (value ? 0xff : 0), bytes);
		}
	}
	//-------------------------------------------------------------------------------
	size_t BitArray::Size() const
	{
		return _size;
	}
	bool BitArray::Empty() const
	{
		return (_size == 0);
	}
	void BitArray::Resize(size_t size)
	{
		if (size == _size)
			return;

		if (size > _capacity)
			Grow(size);

		_size = size;
	}
	void BitArray::Clear()
	{
		Resize(0);
	}
	void BitArray::Reserve(size_t capacity)
	{
		if (capacity > _capacity)
		{
			SetCapacity(capacity);
		}
	}
	void BitArray::Trim()
	{
		SetCapacity(_size);
	}
	//-------------------------------------------------------------------------------
	BitArray::Reference BitArray::At(size_t index)
	{
		Assert(index < _size);
		Assert(_buffer);

		size_t elem_index = index / (sizeof(size_t)* 8);
		size_t bit_index = index % (sizeof(size_t)* 8);

		return BitArray::Reference(_buffer[elem_index], (size_t(1) << bit_index));
	}
	BitArray::ConstReference BitArray::At(size_t index) const
	{
		Assert(index < _size);
		Assert(_buffer);

		size_t elem_index = index / (sizeof(size_t)* 8);
		size_t bit_index = index % (sizeof(size_t)* 8);

		return ((_buffer[elem_index] & (size_t(1) << bit_index)) != 0);
	}
	//-------------------------------------------------------------------------------
	void BitArray::PushBack(bool value)
	{
		if (_size >= _capacity)
			Grow();

		size_t elem_index = _size / (sizeof(size_t) * 8);
		size_t bit_index = _size % (sizeof(size_t) * 8);

		if (value)
		{
			// Set bit
			_buffer[elem_index] |= (size_t(1) << bit_index);
		}
		else
		{
			// Clear bit
			_buffer[elem_index] &= ~(size_t(1) << bit_index);
		}

		++_size;
	}
	void BitArray::PopBack()
	{
		Assert(_size != 0);
		--_size;
	}
	//-------------------------------------------------------------------------------
	BitArray& BitArray::operator=(const BitArray& other)
	{
		// Calculate capacity, align to the number of bits in uint32_t
		_capacity = _size = other._size;

		size_t align = sizeof(size_t) * 8;
		size_t mod = _capacity % align;
		if (mod != 0)
			_capacity += (align - mod);

		if (_capacity != 0)
		{
			size_t bytes = _capacity / 8; // Bytes to allocate
			_buffer = (size_t*)_allocator->Allocate(bytes);
			memcpy(_buffer, other._buffer, (_size + 7) / 8);
		}

		return *this;
	}
	BitArray::Reference BitArray::operator[](size_t index)
	{
		return At(index);
	}
	BitArray::ConstReference BitArray::operator[](size_t index) const
	{
		return At(index);
	}
	//-------------------------------------------------------------------------------
	void BitArray::Grow(size_t capacity)
	{
		capacity = Max(capacity, (_capacity * 2 + 8));
		SetCapacity(capacity);
	}
	void BitArray::SetCapacity(size_t new_capacity)
	{
		if (new_capacity == _capacity)
			return;

		// Align the capacity to the number of bits in uint32_t
		size_t align = sizeof(size_t)* 8;
		size_t mod = new_capacity % align;
		if (mod != 0)
			new_capacity += (align - mod);

		if (new_capacity < _size)
			Resize(new_capacity);

		size_t* new_data = 0;
		if (new_capacity > 0)
		{
			size_t bytes = new_capacity / 8; // Bytes to allocate
			new_data = (size_t*)_allocator->Allocate(bytes);
			memcpy(new_data, _buffer, (_size / 8));
		}
		_allocator->Free(_buffer);
		_buffer = new_data;

		_capacity = new_capacity;
	}
	//-------------------------------------------------------------------------------

} // namespace sb

