// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_RRAWBUFFER_H__
#define __RENDERING_RRAWBUFFER_H__

#include "RenderResource.h"
#include "RHardwareBuffer.h"

namespace sb
{

	struct RawBufferDesc
	{
		// Format of each element when using in shaders
		enum ElementType
		{
			ET_FLOAT4,
			ET_STRUCTURE
		};

		hardware_buffer::Usage usage;
		ElementType elem_type;
		uint32_t elem_size; // Element size, for structured buffer
		uint32_t size; // Total size in bytes
	};

	class RRawBuffer : public RHardwareBuffer
	{
	public:
		RRawBuffer();
		RRawBuffer(const RawBufferDesc& desc);

		uint32_t GetSize() const;

		const RawBufferDesc& GetDesc() const;

	private:
		RawBufferDesc _desc;
	};


} // namespace sb


#endif // __RENDERING_RRAWBUFFER_H__
