// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "D3D11RasterizerStage.h"
#include "D3D11RenderStates.h"
#include "D3D11DeviceContext.h"


namespace sb
{

	//-------------------------------------------------------------------------------

	D3D11RasterizerStage::D3D11RasterizerStage(D3D11DeviceContext* device_context)
		: _device_context(device_context),
		_state(NULL),
		_num_viewports(0),
		_num_rects(0),
		_dirty(true)
	{

	}
	D3D11RasterizerStage::~D3D11RasterizerStage()
	{

	}

	//-------------------------------------------------------------------------------

	void D3D11RasterizerStage::SetRasterizerState(D3D11RasterizerState* state)
	{
		_state = state->GetState();
		_dirty = true;
	}
	void D3D11RasterizerStage::SetViewports(uint32_t num, const D3D11_VIEWPORT* viewports)
	{
		Assert(num < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE);
		memcpy(_viewports, viewports, sizeof(D3D11_VIEWPORT)*num);
		_num_viewports = num;
		_dirty = true;
	}
	void D3D11RasterizerStage::SetScissorRects(uint32_t num, const D3D11_RECT* rects)
	{
		Assert(num < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE);
		memcpy(_scissor_rects, rects, sizeof(D3D11_VIEWPORT)*num);
		_num_rects = num;
		_dirty = true;
	}

	//-------------------------------------------------------------------------------
	void D3D11RasterizerStage::Apply()
	{
		if (_dirty)
		{
			ID3D11DeviceContext* context = _device_context->GetD3DContext();

			context->RSSetState(_state);
			context->RSSetViewports(_num_viewports, _viewports);
			context->RSSetScissorRects(_num_rects, _scissor_rects);

			_dirty = false;
		}
	}
	bool D3D11RasterizerStage::Dirty()
	{
		return _dirty;
	}
	void D3D11RasterizerStage::Clear()
	{
		_state = NULL;
		_dirty = true;
	}

	//-------------------------------------------------------------------------------

} // namespace sb

