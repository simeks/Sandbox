// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RVertexBuffer.h"

namespace sb
{

	RVertexBuffer::RVertexBuffer() : RHardwareBuffer(VERTEX_BUFFER)
	{
	}
	RVertexBuffer::RVertexBuffer(const VertexBufferDesc& desc) : RHardwareBuffer(VERTEX_BUFFER), _desc(desc)
	{
	}
	uint32_t RVertexBuffer::GetSize() const
	{
		return _desc.vertex_size * _desc.vertex_count;
	}

	const VertexBufferDesc& RVertexBuffer::GetDesc() const
	{
		return _desc;
	}

	VertexBufferDesc& RVertexBuffer::GetDesc() 
	{
		return _desc;
	}

} // namespace sb

