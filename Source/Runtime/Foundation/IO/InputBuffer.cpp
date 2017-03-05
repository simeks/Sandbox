// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "InputBuffer.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	InputBuffer::InputBuffer() : _buffer(NULL), _length(0)
	{
	}
	InputBuffer::~InputBuffer()
	{
	}
	void* InputBuffer::Ptr()
	{
		return _buffer;
	}
	int64_t InputBuffer::Length() const
	{
		return _length;
	}
	//-------------------------------------------------------------------------------

} // namespace sb

