// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "D3D11InputAssemblerStage.h"
#include "D3D11DeviceContext.h"
#include "D3D11ResourceManager.h"
#include "D3D11Buffer.h"
#include "D3D11ShaderProgram.h"

#include <Engine/Rendering/RVertexBuffer.h>
#include <Engine/Rendering/RIndexBuffer.h>
#include <Engine/Rendering/RVertexDeclaration.h>


namespace sb
{

	namespace
	{
		// TODO: Move this maybe
		D3D_PRIMITIVE_TOPOLOGY primitive_to_d3d_table[] = {
			/* PRIMITIVE_POINT_LIST = */	D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
			/* PRIMITIVE_LINE_LIST = */		D3D_PRIMITIVE_TOPOLOGY_LINELIST,
			/* PRIMITIVE_LINE_STRIP = */	D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
			/* PRIMITIVE_TRIANGLELIST = */	D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			/* PRIMITIVE_TRIANGLESTRIP = */ D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		};
	}


	D3D11InputAssemblerStage::D3D11InputAssemblerStage(D3D11DeviceContext* device_context)
		: _device_context(device_context), _dirty(true), _prim_type(DrawCall::PRIMITIVE_POINT_LIST)
	{
		Clear();
	}
	D3D11InputAssemblerStage::~D3D11InputAssemblerStage()
	{

	}

	void D3D11InputAssemblerStage::SetVertexBuffers(uint32_t slot, RenderResource* buffer, uint32_t offset)
	{
		Assert(slot < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
		if (buffer && _vb_handles[slot] == buffer->GetHandle() && _vb_offsets[slot] == offset)
			return; // Already bound

		if (buffer && IsValid(buffer->GetHandle()))
		{
			_vb_handles[slot] = buffer->GetHandle();
			_vb_offsets[slot] = offset;
		}
		else
		{
			_vb_handles[slot] = Invalid<uint32_t>();
			_vb_offsets[slot] = 0;
		}
		_dirty = true;
	}
	void D3D11InputAssemblerStage::SetIndexBuffer(RenderResource* buffer, uint32_t offset)
	{
		if (buffer && _ib_handle == buffer->GetHandle() && _ib_offset == offset)
			return; // Already bound

		if (buffer && IsValid(buffer->GetHandle()))
		{
			_ib_handle = buffer->GetHandle();
			_ib_offset = offset;
		}
		else
		{
			_ib_handle = Invalid<uint32_t>();
			_ib_offset = 0;
		}
		_dirty = true;
	}
	void D3D11InputAssemblerStage::SetPrimitiveType(DrawCall::PrimitiveType type)
	{
		_prim_type = type;
	}
	void D3D11InputAssemblerStage::SetVertexDeclaration(RenderResource* declaration)
	{
		if (declaration && _vd_handle == declaration->GetHandle())
			return; // Already bound

		if (declaration && IsValid(declaration->GetHandle()))
		{
			_vd_handle = declaration->GetHandle();
		}
		else
		{
			_vd_handle = Invalid<uint32_t>();
		}
		_dirty = true;
	}
	void D3D11InputAssemblerStage::SetVertexProgram(D3D11ShaderProgram<ID3D11VertexShader>* vertex_program)
	{
		if (vertex_program && vertex_program == _vertex_program)
			return;

		_vertex_program = vertex_program;
		_dirty = true;
	}

	void D3D11InputAssemblerStage::Apply()
	{
		if (!_dirty)
			return;

		ID3D11DeviceContext* context = _device_context->GetD3DContext();

		// Bind input layout

		if (IsValid(_vd_handle) && _vertex_program)
		{
			ID3D11InputLayout* il = _device_context->GetResourceManager()->GetInputLayout(_vd_handle, _vertex_program);
			context->IASetInputLayout(il);
		}

		// Primitive type
		context->IASetPrimitiveTopology(primitive_to_d3d_table[_prim_type]);


		ID3D11Buffer* vb[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		UINT strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

		uint32_t start = 0;
		uint32_t end = 0;

		// Bind vertex streams

		D3D11VertexBuffer* d3dvb = NULL;
		for (uint32_t i = 0; i < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT; ++i)
		{
			if (IsValid(_vb_handles[i]))
			{
				end = Max(end, i);
				d3dvb = _device_context->GetResourceManager()->GetVertexBuffer(_vb_handles[i]);

				vb[i] = d3dvb->buffer.Get();
				strides[i] = d3dvb->vertex_size;
			}
			else
			{
				vb[i] = NULL;
				strides[i] = 0;
			}
		}

		UINT num_buffers = end - start + 1;
		Assert(0 <= num_buffers && num_buffers <= (D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - start));
		context->IASetVertexBuffers(start, num_buffers, &vb[start], &strides[start], &_vb_offsets[start]);

		// Bind index stream

		ID3D11Buffer* ib = NULL;
		DXGI_FORMAT fmt = DXGI_FORMAT_UNKNOWN;
		if (IsValid(_ib_handle))
		{
			D3D11IndexBuffer* d3dib = _device_context->GetResourceManager()->GetIndexBuffer(_ib_handle);
			ib = d3dib->buffer.Get();
			fmt = d3dib->format;
		}

		context->IASetIndexBuffer(ib, fmt, _ib_offset);

		_dirty = false;
	}
	bool D3D11InputAssemblerStage::Dirty()
	{
		return _dirty;
	}
	void D3D11InputAssemblerStage::Clear()
	{
		for (uint32_t i = 0; i < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT; ++i)
		{
			_vb_handles[i] = Invalid<uint32_t>();
			_vb_offsets[i] = 0;
		}

		_ib_handle = Invalid<uint32_t>();
		_ib_offset = 0;

		_vd_handle = Invalid<uint32_t>();
		_vertex_program = NULL;

		_dirty = true;
	}

	uint32_t D3D11InputAssemblerStage::GetVertexBuffer(uint32_t slot)
	{
		Assert(slot < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
		return _vb_handles[slot];
	}
	uint32_t D3D11InputAssemblerStage::GetIndexBuffer()
	{
		return _ib_handle;
	}

} // namespace sb


