// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11BUFFER_H__
#define __D3D11BUFFER_H__

#include "D3D11Resource.h"


namespace sb
{

	struct D3D11Buffer : public D3D11Resource
	{
		D3D11Buffer() { type = BUFFER; }

		ComPtr<ID3D11Buffer> buffer;
	};

	struct D3D11VertexBuffer : public D3D11Buffer
	{
		D3D11VertexBuffer() : D3D11Buffer(), vertex_size(0), vertex_count(0), dynamic(false) {}

		uint32_t vertex_size;
		uint32_t vertex_count;
		bool dynamic;
	};

	struct D3D11IndexBuffer : public D3D11Buffer
	{
		D3D11IndexBuffer() : D3D11Buffer(), format(DXGI_FORMAT_UNKNOWN), index_count(0), dynamic(false) {}

		DXGI_FORMAT format;
		uint32_t index_count;
		bool dynamic;
	};

	struct D3D11ConstantBuffer : public D3D11Buffer
	{
		D3D11ConstantBuffer() : D3D11Buffer(), size(0) {}
		uint32_t size;
	};

	struct D3D11RawBuffer : public D3D11Buffer
	{
		D3D11RawBuffer() : D3D11Buffer(), size(0), srv(NULL) {}
		uint32_t size;

		ComPtr<ID3D11ShaderResourceView> srv;
	};

} // namespace sb


#endif // __D3D11BUFFER_H__

