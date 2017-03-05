// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11TEXTURE_H__
#define __D3D11TEXTURE_H__

#include "D3D11Resource.h"


namespace sb
{

	struct D3D11Texture
	{
		D3D11Resource* data;
		ComPtr<ID3D11ShaderResourceView> srv;
		ComPtr<ID3D11ShaderResourceView> srv_srgb;
		uint32_t size; // Total size in bytes
	};

	struct D3D11Texture1D : public D3D11Resource
	{
		D3D11Texture1D() { type = TEXTURE1D; }

		ComPtr<ID3D11Texture1D> texture;

	};

	struct D3D11Texture2D : public D3D11Resource
	{
		D3D11Texture2D() { type = TEXTURE2D; }

		ComPtr<ID3D11Texture2D> texture;

	};

	struct D3D11Texture3D : public D3D11Resource
	{
		D3D11Texture3D() { type = TEXTURE3D; }

		ComPtr<ID3D11Texture3D> texture;

	};

} // namespace sb

#endif // __D3D11TEXTURE_H__
