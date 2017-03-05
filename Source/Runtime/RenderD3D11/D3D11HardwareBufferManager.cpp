// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "D3D11HardwareBufferManager.h"
#include "D3D11RenderDevice.h"
#include "D3D11DeviceContext.h"
#include "D3D11Buffer.h"
#include "D3D11Statistics.h"

#include <Engine/Rendering/RVertexBuffer.h>
#include <Engine/Rendering/RIndexBuffer.h>
#include <Engine/Rendering/RConstantBuffer.h>
#include <Engine/Rendering/RRawBuffer.h>


namespace sb
{

	//-------------------------------------------------------------------------------
	D3D11HardwareBufferManager::D3D11HardwareBufferManager(D3D11RenderDevice* render_device, D3D11MemoryStatistics* statistics)
		: _render_device(render_device),
		_statistics(statistics)
	{

	}
	D3D11HardwareBufferManager::~D3D11HardwareBufferManager()
	{

	}
	//-------------------------------------------------------------------------------
	void D3D11HardwareBufferManager::CreateVertexBuffer(D3D11VertexBuffer* d3d_buffer, const VertexBufferDesc& desc, void* initial_data)
	{
		d3d_buffer->vertex_count = desc.vertex_count;
		d3d_buffer->vertex_size = desc.vertex_size;
		Assert(d3d_buffer->vertex_count != 0);
		Assert(d3d_buffer->vertex_size != 0);

		D3D11_BUFFER_DESC d3d_desc;
		d3d_desc.ByteWidth = d3d_buffer->vertex_count * d3d_buffer->vertex_size;
		d3d_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		d3d_desc.MiscFlags = 0;
		d3d_desc.StructureByteStride = 0;

		if (desc.usage == hardware_buffer::DYNAMIC) // is dynamic
		{
			d3d_buffer->dynamic = true;

			d3d_desc.Usage = D3D11_USAGE_DYNAMIC;
			d3d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else // if(buffer->GetUsage() == hardware_buffer::IMMUTABLE)
		{
			d3d_buffer->dynamic = false;

			Assert(initial_data); // We need initial data for immutable buffers
			d3d_desc.Usage = D3D11_USAGE_IMMUTABLE;
			d3d_desc.CPUAccessFlags = 0;
		}

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = initial_data;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		D3D_VERIFY(_render_device->GetD3DDevice()->CreateBuffer(&d3d_desc, (initial_data ? &data : NULL), &d3d_buffer->buffer));

		// Update statistics
		_statistics->vertex_buffer_memory += d3d_desc.ByteWidth;
	}
	void D3D11HardwareBufferManager::UpdateVertexBuffer(D3D11DeviceContext* context, D3D11VertexBuffer* d3d_buffer, void* buffer_data)
	{
		// Assumes all buffers are dynamic
		Assert(d3d_buffer->dynamic);

		// Just write the new data to the buffer
		ID3D11DeviceContext* d3d_context = context->GetD3DContext();
		Assert(buffer_data);
		Assert(d3d_buffer->buffer.Get());

		D3D11_MAPPED_SUBRESOURCE data;
		D3D_VERIFY(d3d_context->Map(d3d_buffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &data));

		memcpy(data.pData, buffer_data, d3d_buffer->vertex_count * d3d_buffer->vertex_size);

		d3d_context->Unmap(d3d_buffer->buffer.Get(), 0);
	}
	void D3D11HardwareBufferManager::DestroyVertexBuffer(D3D11VertexBuffer* d3d_buffer)
	{
		// Update statistics
		_statistics->vertex_buffer_memory -= d3d_buffer->vertex_count * d3d_buffer->vertex_size;
	}

	//-------------------------------------------------------------------------------
	void D3D11HardwareBufferManager::CreateIndexBuffer(D3D11IndexBuffer* d3d_buffer, const IndexBufferDesc& desc, void* initial_data)
	{
		d3d_buffer->index_count = desc.index_count;
		Assert(d3d_buffer->index_count != 0);

		uint32_t index_size = 0;
		switch (desc.index_format)
		{
		case index_buffer::INDEX_16:
			d3d_buffer->format = DXGI_FORMAT_R16_UINT;
			index_size = 2;
			break;
		case index_buffer::INDEX_32:
			d3d_buffer->format = DXGI_FORMAT_R32_UINT;
			index_size = 4;
			break;
		default:
			Assert(false);
			break;
		}

		D3D11_BUFFER_DESC d3d_desc;
		d3d_desc.ByteWidth = d3d_buffer->index_count * index_size;
		d3d_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		d3d_desc.MiscFlags = 0;
		d3d_desc.StructureByteStride = 0;

		if (desc.usage == hardware_buffer::DYNAMIC) // is dynamic
		{
			d3d_buffer->dynamic = true;

			d3d_desc.Usage = D3D11_USAGE_DYNAMIC;
			d3d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else // if(buffer->GetUsage() == hardware_buffer::IMMUTABLE)
		{
			d3d_buffer->dynamic = false;

			d3d_desc.Usage = D3D11_USAGE_IMMUTABLE;
			d3d_desc.CPUAccessFlags = 0;
		}

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = initial_data;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		D3D_VERIFY(_render_device->GetD3DDevice()->CreateBuffer(&d3d_desc, &data, &d3d_buffer->buffer));

		// Update statistics
		_statistics->index_buffer_memory += d3d_desc.ByteWidth;
	}
	void D3D11HardwareBufferManager::UpdateIndexBuffer(D3D11DeviceContext* context, D3D11IndexBuffer* d3d_buffer, void* buffer_data)
	{
		// Assumes all buffers are dynamic
		Assert(d3d_buffer->dynamic);

		// Just write the new data to the buffer
		ID3D11DeviceContext* d3d_context = context->GetD3DContext();
		Assert(buffer_data);
		Assert(d3d_buffer->buffer.Get());

		D3D11_MAPPED_SUBRESOURCE data;
		D3D_VERIFY(d3d_context->Map(d3d_buffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &data));

		memcpy(data.pData, buffer_data, d3d_buffer->index_count * (d3d_buffer->format == DXGI_FORMAT_R16_UINT ? 2 : 4));

		d3d_context->Unmap(d3d_buffer->buffer.Get(), 0);

	}
	void D3D11HardwareBufferManager::DestroyIndexBuffer(D3D11IndexBuffer* d3d_buffer)
	{
		// Update statistics
		_statistics->index_buffer_memory -= d3d_buffer->index_count * (d3d_buffer->format == DXGI_FORMAT_R16_UINT ? 2 : 4);
	}

	//-------------------------------------------------------------------------------
	void D3D11HardwareBufferManager::CreateConstantBuffer(D3D11ConstantBuffer* d3d_buffer, uint32_t size, void* initial_data)
	{
		d3d_buffer->size = size;

		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = d3d_buffer->size;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = initial_data;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		D3D_VERIFY(_render_device->GetD3DDevice()->CreateBuffer(&desc, &data, &d3d_buffer->buffer));

		// Update statistics
		_statistics->constant_buffer_memory += desc.ByteWidth;
	}
	void D3D11HardwareBufferManager::UpdateConstantBuffer(D3D11DeviceContext* context, D3D11ConstantBuffer* d3d_buffer, void* buffer_data)
	{
		ID3D11DeviceContext* d3d_context = context->GetD3DContext();
		Assert(buffer_data);
		Assert(d3d_buffer->buffer.Get());

		D3D11_MAPPED_SUBRESOURCE data;
		D3D_VERIFY(d3d_context->Map(d3d_buffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &data));

		memcpy(data.pData, buffer_data, d3d_buffer->size);

		d3d_context->Unmap(d3d_buffer->buffer.Get(), 0);
	}
	void D3D11HardwareBufferManager::DestroyConstantBuffer(D3D11ConstantBuffer* d3d_buffer)
	{
		// Update statistics
		_statistics->constant_buffer_memory -= d3d_buffer->size;
	}
	//-------------------------------------------------------------------------------

	void D3D11HardwareBufferManager::CreateRawBuffer(D3D11RawBuffer* d3d_buffer, const RawBufferDesc& desc, void* initial_data)
	{
		d3d_buffer->size = desc.size;

		D3D11_BUFFER_DESC d3d_desc;
		d3d_desc.ByteWidth = d3d_buffer->size;
		d3d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		d3d_desc.MiscFlags = 0;
		d3d_desc.StructureByteStride = 0;

		// Is this a structure buffer?
		if (desc.elem_type == RawBufferDesc::ET_STRUCTURE)
		{
			d3d_desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			d3d_desc.StructureByteStride = desc.elem_size;
		}


		if (desc.usage == hardware_buffer::DYNAMIC) // is dynamic
		{
			d3d_desc.Usage = D3D11_USAGE_DYNAMIC;
			d3d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else // if(buffer->GetUsage() == hardware_buffer::IMMUTABLE)
		{
			d3d_desc.Usage = D3D11_USAGE_IMMUTABLE;
			d3d_desc.CPUAccessFlags = 0;
		}

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = initial_data;

		D3D_VERIFY(_render_device->GetD3DDevice()->CreateBuffer(&d3d_desc, (initial_data ? &data : NULL), &d3d_buffer->buffer));

		// Update statistics
		_statistics->raw_buffer_memory += d3d_desc.ByteWidth;

		// Create shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srv_desc.Buffer.FirstElement = 0;

		switch (desc.elem_type)
		{
		case RawBufferDesc::ET_FLOAT4:
			srv_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			srv_desc.Buffer.NumElements = d3d_desc.ByteWidth / (sizeof(float)* 4);
			break;
		case RawBufferDesc::ET_STRUCTURE:
			srv_desc.Format = DXGI_FORMAT_UNKNOWN;
			srv_desc.Buffer.NumElements = d3d_desc.ByteWidth / d3d_desc.StructureByteStride;
			break;
		};

		D3D_VERIFY(_render_device->GetD3DDevice()->CreateShaderResourceView(d3d_buffer->buffer.Get(), &srv_desc, &d3d_buffer->srv));
	}
	void D3D11HardwareBufferManager::UpdateRawBuffer(D3D11DeviceContext* context, D3D11RawBuffer* d3d_buffer, void* buffer_data)
	{
		// Just write the new data to the buffer
		ID3D11DeviceContext* d3d_context = context->GetD3DContext();
		Assert(buffer_data);
		Assert(d3d_buffer->buffer.Get());

		D3D11_MAPPED_SUBRESOURCE data;
		D3D_VERIFY(d3d_context->Map(d3d_buffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &data));

		memcpy(data.pData, buffer_data, d3d_buffer->size);

		d3d_context->Unmap(d3d_buffer->buffer.Get(), 0);

	}
	void D3D11HardwareBufferManager::DestroyRawBuffer(D3D11RawBuffer* d3d_buffer)
	{
		_statistics->raw_buffer_memory -= d3d_buffer->size;

	}

	//-------------------------------------------------------------------------------

} // namespace sb

