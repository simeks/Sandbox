// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_RINDEXBUFFER_H__
#define __RENDERING_RINDEXBUFFER_H__

#include "RenderResource.h"
#include "RHardwareBuffer.h"

namespace sb
{

	namespace index_buffer
	{
		enum Format { INDEX_16, INDEX_32 }; // Format (Size in bits)
	};

	struct IndexBufferDesc
	{
		IndexBufferDesc() : index_format(index_buffer::INDEX_32), index_count(0), usage(hardware_buffer::IMMUTABLE) {}

		index_buffer::Format index_format;
		uint32_t index_count;

		hardware_buffer::Usage usage;
	};

	class RIndexBuffer : public RHardwareBuffer
	{
	public:
		RIndexBuffer();
		RIndexBuffer(const IndexBufferDesc& desc);

		uint32_t GetSize() const;

		const IndexBufferDesc& GetDesc() const;

	private:
		IndexBufferDesc _desc;
	};

} // namespace sb


#endif // __RENDERING_RINDEXBUFFER_H__

