// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "MemoryStream.h"


namespace sb
{

	//------------------------------------------------------------------------------
	StaticMemoryStream::StaticMemoryStream(void* data, size_t size)
		: _start((uint8_t*)data),
		_current((uint8_t*)data),
		_end((uint8_t*)data + size)
	{
	}
	StaticMemoryStream::~StaticMemoryStream()
	{
	}
	//------------------------------------------------------------------------------
	size_t StaticMemoryStream::Read(void* dst, size_t size)
	{
		Assert(dst);
		Assert(_current);

		size_t read_bytes = Min<size_t>(size, size_t(_end - _current));

		memcpy(dst, _current, read_bytes);
		_current += read_bytes;
		return read_bytes;
	}
	size_t StaticMemoryStream::Write(const void* src, size_t size)
	{
		Assert(src);
		Assert(_current);

		size_t write_bytes = Min<size_t>(size, size_t(_end - _current));

		memcpy(_current, src, write_bytes);
		_current += write_bytes;
		return write_bytes;
	}
	//------------------------------------------------------------------------------
	uint8_t* StaticMemoryStream::Current()
	{
		return _current;
	}
	//------------------------------------------------------------------------------
	int64_t StaticMemoryStream::Seek(int64_t offset)
	{
		Assert((_start + offset) <= _end && (_start + offset) >= _start);
		_current = _start + offset;

		return (_current - _start);
	}
	int64_t StaticMemoryStream::Tell() const
	{
		return _current - _start;
	}
	bool StaticMemoryStream::Eof() const
	{
		return (_current == _end);
	}
	int64_t StaticMemoryStream::Length() const
	{
		return (_end - _start);
	}
	//------------------------------------------------------------------------------
	bool StaticMemoryStream::Readable() const
	{
		return true;
	}
	bool StaticMemoryStream::Writeable() const
	{
		return true;
	}
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	DynamicMemoryStream::DynamicMemoryStream(vector<uint8_t>* buffer) : _buffer(buffer), _offset(0)
	{
	}
	DynamicMemoryStream::~DynamicMemoryStream()
	{
	}
	//------------------------------------------------------------------------------
	size_t DynamicMemoryStream::Read(void* dst, size_t size)
	{
		Assert(_buffer);

		size_t read_bytes = Min<size_t>(size, (_buffer->size() - _offset));

		memcpy(dst, _buffer->data() + _offset, read_bytes);
		_offset += read_bytes;

		return read_bytes;
	}
	size_t DynamicMemoryStream::Write(const void* src, size_t size)
	{
		Assert(_buffer);

		// Write what the buffer can hold first
		size_t write_bytes = Min<size_t>(size, (_buffer->size() - _offset));

		memcpy(_buffer->data() + _offset, src, write_bytes);

		// Then check if we have any data remaining
		size_t remaining_bytes = size - write_bytes;
		if (remaining_bytes != 0)
		{
			size_t cur_size = _buffer->size();

			// If we do we have to resize our buffer
			_buffer->resize(cur_size + remaining_bytes);
			memcpy(_buffer->data() + cur_size, (const uint8_t*)src + write_bytes, remaining_bytes);

		}

		_offset += size;

		return size;
	}
	//------------------------------------------------------------------------------
	int64_t DynamicMemoryStream::Seek(int64_t offset)
	{
		Assert(_buffer);

		_offset = (uint32_t)offset;

		if (_offset > _buffer->size())
		{
			_buffer->insert(_buffer->end(), (_offset - _buffer->size()), 0);
		}

		return _offset;
	}
	int64_t DynamicMemoryStream::Tell() const
	{
		Assert(_buffer);
		return _offset;
	}
	bool DynamicMemoryStream::Eof() const
	{
		// No end in a dynamic buffer
		return false;
	}
	int64_t DynamicMemoryStream::Length() const
	{
		Assert(_buffer);
		return _buffer->size();
	}
	//------------------------------------------------------------------------------
	bool DynamicMemoryStream::Readable() const
	{
		return true;
	}
	bool DynamicMemoryStream::Writeable() const
	{
		return true;
	}
	//------------------------------------------------------------------------------

} // namespace sb

