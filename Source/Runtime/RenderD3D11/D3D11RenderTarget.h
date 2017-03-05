// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11RENDERTARGET_H__
#define __D3D11RENDERTARGET_H__

#include "D3D11Resource.h"


namespace sb
{

	struct D3D11RenderTarget
	{
		D3D11RenderTarget()
			: target(nullptr), rtv(nullptr), dsv(nullptr), srv(nullptr),
			uav(nullptr), size(0), depth_stencil(false)
		{
		}

		// Target resource
		D3D11Resource* target;

		// Views
		ComPtr<ID3D11RenderTargetView> rtv;
		ComPtr<ID3D11DepthStencilView> dsv;
		ComPtr<ID3D11ShaderResourceView> srv;
		ComPtr<ID3D11UnorderedAccessView> uav;

		bool depth_stencil;	///< Specifies if this is a depth stencil
		uint32_t size; ///< Size in bytes
	};

} // namespace sb


#endif // __D3D11RENDERTARGET_H__

