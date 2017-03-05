// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RIndexBuffer.h"

namespace sb
{

	RIndexBuffer::RIndexBuffer() : RHardwareBuffer(INDEX_BUFFER)
	{

	}
	RIndexBuffer::RIndexBuffer(const IndexBufferDesc& desc) : RHardwareBuffer(INDEX_BUFFER), _desc(desc)
	{

	}
	uint32_t RIndexBuffer::GetSize() const
	{
		return _desc.index_count * (_desc.index_format == index_buffer::INDEX_32 ? 4 : 2);
	}

	const IndexBufferDesc& RIndexBuffer::GetDesc() const
	{
		return _desc;
	}


} // namespace sb
