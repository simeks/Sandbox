// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "StringStream.h"

namespace sb
{

	StringStream::StringStream(Allocator& allocator) : _buffer(allocator)
	{
	}
	StringStream::~StringStream()
	{
	}

	StringStream& StringStream::operator<<(int val)
	{
		char buf[12];

		snprintf(buf, sizeof(buf), "%d", val);
		_buffer += buf;

		return *this;
	}
	StringStream& StringStream::operator<<(uint32_t val)
	{
		char buf[12];

		snprintf(buf, sizeof(buf), "%u", val);
		_buffer += buf;

		return *this;
	}
	StringStream& StringStream::operator<<(int64_t val)
	{
		char buf[22];

		snprintf(buf, sizeof(buf), "%lld", val);
		_buffer += buf;

		return *this;
	}
	StringStream& StringStream::operator<<(uint64_t val)
	{
		char buf[22];

		snprintf(buf, sizeof(buf), "%llu", val);
		_buffer += buf;

		return *this;
	}
	StringStream& StringStream::operator<<(float val)
	{
		char buf[32];
		snprintf(buf, sizeof(buf), "%.4g", val);
		_buffer += buf;

		return *this;
	}
	StringStream& StringStream::operator<<(double val)
	{
		char buf[32];
		snprintf(buf, sizeof(buf), "%.16g", val);
		_buffer += buf;

		return *this;
	}
	StringStream& StringStream::operator<<(const char* val)
	{
		_buffer += val;
		return *this;
	}
	StringStream& StringStream::operator<<(char val)
	{
		_buffer.Append(val);
		return *this;
	}
	StringStream& StringStream::operator<<(bool val)
	{
		if (val)
			_buffer += "true";
		else
			_buffer += "false";
		return *this;
	}

	void StringStream::Clear()
	{
		_buffer.Clear();
	}
	size_t StringStream::Size() const
	{
		return _buffer.Size();
	}
	const String& StringStream::Str() const
	{
		return _buffer;
	}
	const char* StringStream::Ptr() const
	{
		return _buffer.Ptr();
	}
	//-------------------------------------------------------------------------------

} // namespace sb


