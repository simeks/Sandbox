// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_RRENDERTARGET_H__
#define __RENDERING_RRENDERTARGET_H__

#include "RenderResource.h"
#include "RTexture.h"

namespace sb
{

	/// Texture render resource
	class RRenderTarget : public RenderResource
	{
	public:
		enum BindFlags
		{
			BIND_RENDER_TARGET = 0x1,
			BIND_DEPTH_STENCIL = 0x2,	///< Indicates that this render target are going to be used as a depth stencil
			BIND_UNORDERED_ACCESS = 0x4
		};

		RRenderTarget();
		RRenderTarget(const TextureDesc& desc, uint32_t bind_flags = 0);

		const TextureDesc& GetDesc() const;
		uint32_t GetBindFlags() const;

	private:
		TextureDesc _desc;
		uint32_t _bind_flags;
	};

} // namespace sb


#endif // __RENDERING_RRENDERTARGET_H__

