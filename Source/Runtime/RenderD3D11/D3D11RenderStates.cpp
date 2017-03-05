// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "D3D11RenderStates.h"
#include "D3D11ResourceManager.h"


namespace sb
{

	//-------------------------------------------------------------------------------

	D3D11BlendState::D3D11BlendState()
		: _state(NULL)
	{
	}
	D3D11BlendState::~D3D11BlendState()
	{
	}

	void D3D11BlendState::Create(D3D11ResourceManager* resource_manager, const D3D11_BLEND_DESC& desc, float* blend_factor, uint32_t sample_mask)
	{
		memcpy(&_desc, &desc, sizeof(D3D11_BLEND_DESC));
		memcpy(_blend_factor, blend_factor, sizeof(float)* 4);
		_sample_mask = sample_mask;
		_state = resource_manager->GetBlendState(_desc);
	}
	ID3D11BlendState* D3D11BlendState::GetState() const
	{
		return _state;
	}
	float const*  D3D11BlendState::GetBlendFactor() const
	{
		return &_blend_factor[0];
	}
	uint32_t D3D11BlendState::GetSampleMask() const
	{
		return _sample_mask;
	}

	void D3D11BlendState::SetDefault()
	{
		_desc.AlphaToCoverageEnable = false;
		_desc.IndependentBlendEnable = false;

		for (uint32_t i = 0; i < 8; ++i)
		{
			_desc.RenderTarget[i].BlendEnable = false;
			_desc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
			_desc.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
			_desc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			_desc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
			_desc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
			_desc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			_desc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
	}

	//-------------------------------------------------------------------------------

	D3D11DepthStencilState::D3D11DepthStencilState()
		: _state(NULL)
	{

	}
	D3D11DepthStencilState::~D3D11DepthStencilState()
	{
	}

	void D3D11DepthStencilState::Create(D3D11ResourceManager* resource_manager, const D3D11_DEPTH_STENCIL_DESC& desc, uint32_t stencil_ref)
	{
		memcpy(&_desc, &desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		_stencil_ref = stencil_ref;
		_state = resource_manager->GetDepthStencilState(_desc);
	}
	ID3D11DepthStencilState* D3D11DepthStencilState::GetState() const
	{
		return _state;
	}
	uint32_t D3D11DepthStencilState::GetStencilRef() const
	{
		return _stencil_ref;
	}

	void D3D11DepthStencilState::SetDefault()
	{
		_desc.DepthEnable = true;
		_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		_desc.DepthFunc = D3D11_COMPARISON_LESS;
		_desc.StencilEnable = FALSE;
		_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

		_desc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
		_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	}

	//-------------------------------------------------------------------------------

	D3D11RasterizerState::D3D11RasterizerState()
		: _state(NULL)
	{

	}
	D3D11RasterizerState::~D3D11RasterizerState()
	{
	}

	void D3D11RasterizerState::Create(D3D11ResourceManager* resource_manager, const D3D11_RASTERIZER_DESC& desc)
	{
		memcpy(&_desc, &desc, sizeof(D3D11_RASTERIZER_DESC));
		_state = resource_manager->GetRasterizerState(_desc);
	}
	ID3D11RasterizerState* D3D11RasterizerState::GetState() const
	{
		return _state;
	}
	void D3D11RasterizerState::SetDefault()
	{
		_desc.FillMode = D3D11_FILL_SOLID;
		_desc.CullMode = D3D11_CULL_BACK;
		_desc.FrontCounterClockwise = false;
		_desc.DepthBias = 0;
		_desc.SlopeScaledDepthBias = 0.0f;
		_desc.DepthBiasClamp = 0.0f;
		_desc.DepthClipEnable = true;
		_desc.ScissorEnable = false;
		_desc.MultisampleEnable = false;
		_desc.AntialiasedLineEnable = false;
	}

	//-------------------------------------------------------------------------------

	D3D11SamplerState::D3D11SamplerState()
		: _state(NULL)
	{

	}
	D3D11SamplerState::~D3D11SamplerState()
	{
	}

	void D3D11SamplerState::Create(D3D11ResourceManager* resource_manager, const D3D11_SAMPLER_DESC& desc)
	{
		memcpy(&_desc, &desc, sizeof(D3D11_SAMPLER_DESC));

		_state = resource_manager->GetSamplerState(_desc);
	}
	ID3D11SamplerState* D3D11SamplerState::GetState() const
	{
		return _state;
	}
	void D3D11SamplerState::SetDefault()
	{
		_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		_desc.MipLODBias = 0.0f;
		_desc.MaxAnisotropy = 1;
		_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		for (uint32_t i = 0; i < 4; ++i)
			_desc.BorderColor[i] = 0.0f;
		_desc.MinLOD = 0;
		_desc.MaxLOD = D3D11_FLOAT32_MAX;
	}

	//-------------------------------------------------------------------------------

} // namespace sb

