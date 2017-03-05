// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_RCONSTANTBUFFER_H__
#define __RENDERING_RCONSTANTBUFFER_H__

#include "RHardwareBuffer.h"

namespace sb
{

	class RConstantBuffer : public RHardwareBuffer
	{
	public:
		enum Type
		{
			TYPE_LOCAL, // Local constant buffer, for per-object constants
			TYPE_GLOBAL, // Global constant buffer, for per-view constants
		};

		RConstantBuffer();
		RConstantBuffer(uint32_t size, Type type);

		uint32_t GetSize() const;
		Type GetType() const;

	private:
		uint32_t _size;
		Type _type;
	};

} // namespace sb


#endif // __RENDERING_RCONSTANTBUFFER_H__

