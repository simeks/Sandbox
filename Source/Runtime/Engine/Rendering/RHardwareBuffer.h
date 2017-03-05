// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_RHARDWAREBUFFER_H__
#define __RENDERING_RHARDWAREBUFFER_H__

#include "RenderResource.h"

namespace sb
{

	namespace hardware_buffer
	{
		enum Usage
		{
			IMMUTABLE,
			DYNAMIC
		};
	};


	class RHardwareBuffer : public RenderResource
	{
	public:
		RHardwareBuffer(RenderResource::Type type) : RenderResource(type) {}
		virtual ~RHardwareBuffer() {}

		/// Returns the total size of the buffer in bytes.
		virtual uint32_t GetSize() const = 0;

	};

} // namespace sb


#endif // __RENDERING_RHARDWAREBUFFER_H__
