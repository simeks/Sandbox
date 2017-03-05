// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RRenderTarget.h"

namespace sb
{

	RRenderTarget::RRenderTarget()
		: RenderResource(RENDER_TARGET),
		_bind_flags(0)
	{
	}
	RRenderTarget::RRenderTarget(const TextureDesc& desc, uint32_t bind_flags)
		: RenderResource(RENDER_TARGET),
		_desc(desc),
		_bind_flags(bind_flags)
	{
	}

	const TextureDesc& RRenderTarget::GetDesc() const
	{
		return _desc;
	}
	uint32_t RRenderTarget::GetBindFlags() const
	{
		return _bind_flags;
	}

} // namespace sb

