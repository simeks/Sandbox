// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RenderDevice.h"
#include "RenderContext.h"
#include "RenderResourceAllocator.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	RenderDevice::RenderDevice()
		: _active_swap_chain(Invalid<uint32_t>())
	{
		_resource_allocator = new RenderResourceAllocator(this);
	}
	RenderDevice::~RenderDevice()
	{
		delete _resource_allocator;
		_resource_allocator = nullptr;
	}
	//-------------------------------------------------------------------------------
	void RenderDevice::SetActiveSwapChain(uint32_t swap_chain)
	{
		_active_swap_chain = swap_chain;
	}
	//-------------------------------------------------------------------------------
	RenderContext* RenderDevice::CreateRenderContext()
	{
		ScopedLock<CriticalSection> lock(_context_lock);
		RenderContext* context = new (_render_context_pool.Allocate()) RenderContext();
		return context;
	}
	void RenderDevice::ReleaseRenderContext(RenderContext* context)
	{
		ScopedLock<CriticalSection> lock(_context_lock);
		context->~RenderContext();
		_render_context_pool.Release(context);
	}
	//-------------------------------------------------------------------------------

	RenderResourceAllocator* RenderDevice::GetResourceAllocator()
	{
		return _resource_allocator;
	}

	//-------------------------------------------------------------------------------

} // namespace sb

