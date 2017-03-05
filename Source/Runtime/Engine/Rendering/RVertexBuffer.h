// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_VERTEXBUFFER_H__
#define __RENDERING_VERTEXBUFFER_H__

#include "RenderResource.h"
#include "RHardwareBuffer.h"

namespace sb
{

	struct VertexBufferDesc
	{
		VertexBufferDesc() : vertex_size(0), vertex_count(0), usage(hardware_buffer::IMMUTABLE) {}

		uint32_t vertex_size;
		uint32_t vertex_count;

		hardware_buffer::Usage usage;
	};

	class RVertexBuffer : public RHardwareBuffer
	{
	public:
		RVertexBuffer();
		RVertexBuffer(const VertexBufferDesc& desc);

		uint32_t GetSize() const;

		const VertexBufferDesc& GetDesc() const;
		VertexBufferDesc& GetDesc();

	private:
		VertexBufferDesc _desc;
	};

} // namespace sb



#endif // __RENDERING_VERTEXBUFFER_H__

