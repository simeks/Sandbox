// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RRawBuffer.h"

namespace sb
{

	RRawBuffer::RRawBuffer() : RHardwareBuffer(RAW_BUFFER)
	{
	}
	RRawBuffer::RRawBuffer(const RawBufferDesc& desc) : RHardwareBuffer(RAW_BUFFER), _desc(desc)
	{
	}

	uint32_t RRawBuffer::GetSize() const
	{
		return _desc.size;
	}

	const RawBufferDesc& RRawBuffer::GetDesc() const
	{
		return _desc;
	}

} // namespace sb

