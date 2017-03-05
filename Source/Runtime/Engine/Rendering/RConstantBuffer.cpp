// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RConstantBuffer.h"
#include <Foundation/IO/Stream.h>

namespace sb
{

	RConstantBuffer::RConstantBuffer()
		: RHardwareBuffer(CONSTANT_BUFFER),
		_size(0),
		_type(TYPE_LOCAL)
	{
	}
	RConstantBuffer::RConstantBuffer(uint32_t size, Type type)
		: RHardwareBuffer(CONSTANT_BUFFER),
		_size(size),
		_type(type)
	{
	}

	uint32_t RConstantBuffer::GetSize() const
	{
		return _size;
	}
	RConstantBuffer::Type RConstantBuffer::GetType() const
	{
		return _type;
	}

} // namespace sb

