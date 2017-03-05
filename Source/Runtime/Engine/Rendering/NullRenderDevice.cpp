// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "NullRenderDevice.h"
#include "RenderContext.h"
#include "RRenderTarget.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	NullRenderDevice::NullRenderDevice()
	{
	}
	NullRenderDevice::~NullRenderDevice()
	{
	}
	//-------------------------------------------------------------------------------
	void NullRenderDevice::Initialize(const InitParams&)
	{
	}
	void NullRenderDevice::Shutdown()
	{
	}

	uint32_t NullRenderDevice::CreateSwapChain(Window*, bool)
	{
		return Invalid<uint32_t>();
	}
	void NullRenderDevice::ReleaseSwapChain(uint32_t)
	{
	}

	void NullRenderDevice::Present()
	{
	}

	void NullRenderDevice::Dispatch(uint32_t, RenderContext**)
	{
	}
	void NullRenderDevice::FlushAllocator()
	{
	}

	uint32_t NullRenderDevice::EnumDisplayFormats(uint32_t, DisplayFormat*)
	{
		return 0;
	}
	DisplayFormat NullRenderDevice::GetDesktopFormat()
	{
		DisplayFormat fmt;
		fmt.width = fmt.height = 0;
		return fmt;
	}
	RRenderTarget* NullRenderDevice::GetBackBuffer()
	{
		return nullptr;
	}
	//-------------------------------------------------------------------------------

} // namespace sb

