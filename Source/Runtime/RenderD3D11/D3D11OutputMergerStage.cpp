// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "D3D11OutputMergerStage.h"
#include "D3D11DeviceContext.h"
#include "D3D11Resource.h"
#include "D3D11RenderTarget.h"
#include "D3D11ResourceManager.h"
#include "D3D11RenderStates.h"
#include "D3D11Shader.h"

#include <Engine/Rendering/RRenderTarget.h>



namespace sb
{

	//-------------------------------------------------------------------------------
	D3D11OutputMergerStage::D3D11OutputMergerStage(D3D11DeviceContext* device_context)
		: _device_context(device_context),
		_blend_state(NULL),
		_depth_stencil_state(NULL),
		_target_dirty(true),
		_blend_state_dirty(true),
		_depth_stencil_state_dirty(true),
		_uav_start_slot(0),
		_uav_count(0)
	{
		Clear();
	}
	D3D11OutputMergerStage::~D3D11OutputMergerStage()
	{

	}
	//-------------------------------------------------------------------------------
	void D3D11OutputMergerStage::SetRenderTarget(uint32_t index, uint32_t render_target)
	{
		if (IsValid(render_target) && _render_targets[index] == render_target)
			return; // Already bound

		if (IsValid(render_target))
		{
			_render_targets[index] = render_target;

			D3D11RenderTarget* d3d_target = _device_context->GetResourceManager()->GetRenderTarget(render_target);
			Assert(d3d_target);

			_rtv[index] = d3d_target->rtv.Get();
			_render_target_uav[index] = d3d_target->uav.Get();
		}
		else
		{
			_render_targets[index] = Invalid<uint32_t>();
			_rtv[index] = NULL;
		}

		_target_dirty = true;
	}
	void D3D11OutputMergerStage::SetDepthStencilTarget(uint32_t depth_target)
	{
		if (IsValid(depth_target) && _depth_stencil_target == depth_target)
			return; // Already bound

		if (IsValid(depth_target))
		{
			_depth_stencil_target = depth_target;

			D3D11RenderTarget* d3d_target = _device_context->GetResourceManager()->GetRenderTarget(depth_target);
			Assert(d3d_target);
			Assert(d3d_target->depth_stencil);

			_dsv = d3d_target->dsv.Get();
		}
		else
		{
			_depth_stencil_target = Invalid<uint32_t>();
			_dsv = NULL;
		}

		_target_dirty = true;
	}
	bool D3D11OutputMergerStage::Dirty()
	{
		return (_target_dirty || _blend_state_dirty || _depth_stencil_state_dirty);
	}
	void D3D11OutputMergerStage::Clear()
	{
		for (uint32_t i = 0; i < MAX_MULTIPLE_RENDER_TARGETS; ++i)
		{
			_render_targets[i] = Invalid<uint32_t>();
			_rtv[i] = NULL;
			_render_target_uav[i] = NULL;
		}
		_depth_stencil_target = Invalid<uint32_t>();
		_dsv = NULL;

		_blend_state = NULL;
		_depth_stencil_state = NULL;

		_target_dirty = _blend_state_dirty = _depth_stencil_state_dirty = true;

		_uav.clear();
		_uav_start_slot = 0;
		_uav_count = 0;
	}
	//------------------------------------------------------------------------------
	void D3D11OutputMergerStage::SetBlendState(D3D11BlendState* state)
	{
		float const* blend_factor = state->GetBlendFactor();
		if (_blend_state != state->GetState() || _sample_mask != state->GetSampleMask() ||
			_blend_factor[0] != blend_factor[0] || _blend_factor[1] != blend_factor[1] ||
			_blend_factor[2] != blend_factor[2] || _blend_factor[3] != blend_factor[3])
		{
			_blend_state = state->GetState();
			_sample_mask = state->GetSampleMask();
			memcpy(_blend_factor, blend_factor, sizeof(float)* 4);

			_blend_state_dirty = true;
		}
	}
	void D3D11OutputMergerStage::SetDepthStencilState(D3D11DepthStencilState* state)
	{
		if (_depth_stencil_state != state->GetState() || _stencil_ref != state->GetStencilRef())
		{
			_depth_stencil_state = state->GetState();
			_stencil_ref = state->GetStencilRef();
			_depth_stencil_state_dirty = true;
		}
	}
	void D3D11OutputMergerStage::BindUnorderedAccessViews(RenderResource* resources, uint32_t resource_count, const D3D11ShaderPass* shader_pass)
	{
		D3D11ResourceManager* resource_manager = _device_context->GetResourceManager();
		const D3D11ShaderPassData* pass_data = shader_pass->GetData();

		uint32_t stop = 0;
		_uav.clear();

		for (auto& bind_info : pass_data->compute_shader.resource_bind_info)
		{
			if (bind_info.flags & D3D11ShaderPassData::TEXTURE_UAV)
			{
				Verify(bind_info.index < resource_count);
				if (_uav.size() < (bind_info.bind_point + bind_info.bind_count + 1))
					_uav.insert(_uav.end(), (bind_info.bind_point + bind_info.bind_count + 1) - _uav.size(), NULL);

				ID3D11UnorderedAccessView* view = NULL;
				uint32_t handle = resources[bind_info.index].GetHandle();
				if (IsValid(handle))
					view = resource_manager->GetUnorderedAccessView(resources[bind_info.index].GetHandle());

				_uav_start_slot = Min(_uav_start_slot, bind_info.bind_point);
				stop = Max(stop, bind_info.bind_point + bind_info.bind_count);

				if (_uav[bind_info.bind_point] != view)
				{
					_target_dirty = true;
					_uav[bind_info.bind_point] = view;
				}
			}
		}
		_uav_count = stop - _uav_start_slot;

		_uav_initial_counts.clear();
		_uav_initial_counts.insert(_uav_initial_counts.begin(), _uav.size(), 0);
	}
	//------------------------------------------------------------------------------
	void D3D11OutputMergerStage::ApplyPixelStage()
	{
		// TODO: UAV support for pixel shaders
		ID3D11DeviceContext* context = _device_context->GetD3DContext();
		if (_target_dirty)
		{
			// Unbind any compute shader UAVs incase there's any bound render target UAVs
			ID3D11UnorderedAccessView* null_view = NULL;
			context->CSSetUnorderedAccessViews(0, 1, &null_view, 0);

			context->OMSetRenderTargets(MAX_MULTIPLE_RENDER_TARGETS, _rtv, _dsv);

			_target_dirty = false;
		}
		if (_blend_state_dirty)
		{
			context->OMSetBlendState(_blend_state, _blend_factor, _sample_mask);
			_blend_state_dirty = false;
		}
		if (_depth_stencil_state_dirty)
		{
			context->OMSetDepthStencilState(_depth_stencil_state, _stencil_ref);
			_depth_stencil_state_dirty = false;
		}
	}
	void D3D11OutputMergerStage::ApplyComputeStage()
	{
		ID3D11DeviceContext* context = _device_context->GetD3DContext();
		// Try binding any bound UAV render targets
		for (uint32_t i = 0; i < MAX_MULTIPLE_RENDER_TARGETS; ++i)
		{
			if (i >= _uav.size())
				break;

			if (_render_target_uav[i] != NULL)
			{
				_uav[i] = _render_target_uav[i];

				if (_uav_start_slot > i)
					_uav_start_slot = i;
				_uav_start_slot = Min(_uav_start_slot, i);
				_uav_count = Max((i - _uav_start_slot + 1), _uav_count);
			}
		}

		Assert(0 <= _uav_start_slot && _uav_start_slot < D3D11_1_UAV_SLOT_COUNT);
		Assert(0 <= _uav_count && _uav_count <= (D3D11_1_UAV_SLOT_COUNT - _uav_start_slot));
		context->CSSetUnorderedAccessViews(_uav_start_slot, _uav_count,
			_uav.data() + _uav_start_slot, _uav_initial_counts.data() + _uav_start_slot);

	}
	//------------------------------------------------------------------------------
	ID3D11RenderTargetView** D3D11OutputMergerStage::GetRenderTargetViews()
	{
		return _rtv;
	}
	ID3D11DepthStencilView* D3D11OutputMergerStage::GetDepthStencilView()
	{
		return _dsv;
	}
	//------------------------------------------------------------------------------

} // namespace sb


