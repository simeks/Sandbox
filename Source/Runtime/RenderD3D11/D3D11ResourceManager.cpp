// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "D3D11ResourceManager.h"
#include "D3D11HardwareBufferManager.h"
#include "D3D11ShaderProgramManager.h"
#include "D3D11TextureManager.h"
#include "D3D11Resource.h"
#include "D3D11Shader.h"
#include "D3D11ShaderProgram.h"
#include "D3D11Texture.h"
#include "D3D11Buffer.h"
#include "D3D11VertexDeclaration.h"
#include "D3D11RenderTarget.h"
#include "D3D11RenderDevice.h"
#include "D3D11DeviceContext.h"


#include <Engine/Rendering/RTexture.h>
#include <Engine/Rendering/RRenderTarget.h>
#include <Engine/Rendering/RVertexBuffer.h>
#include <Engine/Rendering/RIndexBuffer.h>
#include <Engine/Rendering/RConstantBuffer.h>
#include <Engine/Rendering/RRawBuffer.h>
#include <Engine/Rendering/RShader.h>
#include <Engine/Rendering/RVertexDeclaration.h>
#include <Engine/Rendering/RenderResourceAllocator.h>

#include <Foundation/Resource/ResourceManager.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Hash/murmur_hash.h>



namespace sb
{

	namespace vertex_declaration_helper
	{
		static const char* element_semantic_to_string_table[] = {
			/* ES_POSITION	*/ "POSITION",
			/* ES_NORMAL	*/ "NORMAL",
			/* ES_TANGENT	*/ "TANGENT",
			/* ES_BINORMAL	*/ "BINORMAL",
			/* ES_TEXCOORD	*/ "TEXCOORD",
			/* ES_COLOR	*/ "COLOR"
		};

		static DXGI_FORMAT element_type_to_dxgi_format[] = {
			/* ET_FLOAT1 */ DXGI_FORMAT_R32_FLOAT,
			/* ET_FLOAT2 */ DXGI_FORMAT_R32G32_FLOAT,
			/* ET_FLOAT3 */ DXGI_FORMAT_R32G32B32_FLOAT,
			/* ET_FLOAT4 */ DXGI_FORMAT_R32G32B32A32_FLOAT,
			/* ET_UBYTE4 */ DXGI_FORMAT_R8G8B8A8_UINT
		};
	};

	bool D3D11_BLEND_DESC_COMPARE::operator()(const D3D11_BLEND_DESC& a, const D3D11_BLEND_DESC& b) const
	{
		uint64_t ha = murmur_hash_64(&a, sizeof(D3D11_BLEND_DESC), 0);
		uint64_t hb = murmur_hash_64(&b, sizeof(D3D11_BLEND_DESC), 0);

		return (ha < hb);
	}

	bool D3D11_DEPTH_STENCIL_DESC_COMPARE::operator()(const D3D11_DEPTH_STENCIL_DESC& a, const D3D11_DEPTH_STENCIL_DESC& b) const
	{
		uint64_t ha = murmur_hash_64(&a, sizeof(D3D11_DEPTH_STENCIL_DESC), 0);
		uint64_t hb = murmur_hash_64(&b, sizeof(D3D11_DEPTH_STENCIL_DESC), 0);

		return (ha < hb);
	}

	bool D3D11_RASTERIZER_DESC_COMPARE::operator()(const D3D11_RASTERIZER_DESC& a, const D3D11_RASTERIZER_DESC& b) const
	{
		uint64_t ha = murmur_hash_64(&a, sizeof(D3D11_RASTERIZER_DESC), 0);
		uint64_t hb = murmur_hash_64(&b, sizeof(D3D11_RASTERIZER_DESC), 0);

		return (ha < hb);
	}

	bool D3D11_SAMPLER_DESC_COMPARE::operator()(const D3D11_SAMPLER_DESC& a, const D3D11_SAMPLER_DESC& b) const
	{
		uint64_t ha = murmur_hash_64(&a, sizeof(D3D11_SAMPLER_DESC), 0);
		uint64_t hb = murmur_hash_64(&b, sizeof(D3D11_SAMPLER_DESC), 0);

		return (ha < hb);
	}


	//-------------------------------------------------------------------------------
	D3D11ResourceManager::D3D11ResourceManager(D3D11RenderDevice* render_device)
		: _device(render_device),
		_num_allocations(0)
	{
		_memory_statistics.render_target_memory = 0;
		_memory_statistics.texture_memory = 0;
		_memory_statistics.vertex_buffer_memory = 0;
		_memory_statistics.index_buffer_memory = 0;
		_memory_statistics.constant_buffer_memory = 0;
		_memory_statistics.raw_buffer_memory = 0;

		_hw_buffer_manager = new D3D11HardwareBufferManager(_device, &_memory_statistics);
		_shader_program_manager = new D3D11ShaderProgramManager(_device);
		_texture_manager = new D3D11TextureManager(_device, &_memory_statistics);
	}
	D3D11ResourceManager::~D3D11ResourceManager()
	{
		ClearStates();

		delete _texture_manager;
		delete _shader_program_manager;
		delete _hw_buffer_manager;

		AssertMsg(_num_allocations == 0, "Unreleased resources");
	}
	//-------------------------------------------------------------------------------
	void D3D11ResourceManager::ClearStates()
	{
		_blend_states.clear();
		_depth_stencil_states.clear();
		_depth_stencil_states.clear();
		_sampler_states.clear();
	}
	//-------------------------------------------------------------------------------
	void D3D11ResourceManager::FlushAllocator(RenderResourceAllocator* resource_allocator)
	{
		resource_allocator->Lock();

		const vector<uint8_t>& cmd_buffer = resource_allocator->CommandBuffer();
		const uint8_t* cur = cmd_buffer.data();
		const uint8_t* end = cur + cmd_buffer.size();
		while (cur != end)
		{
			switch (*cur++)
			{
			case RenderResourceAllocator::ALLOCATE_VERTEX_BUFFER:
			{
				RenderResourceAllocator::AllocateVertexBufferCmd* cmd = (RenderResourceAllocator::AllocateVertexBufferCmd*)cur;
				cur += sizeof(RenderResourceAllocator::AllocateVertexBufferCmd);

				AllocateVertexBuffer(cmd->resource_handle, cmd->desc, cmd->data);
				break;
			}
			case RenderResourceAllocator::RELEASE_VERTEX_BUFFER:
			{
				RenderResourceAllocator::ResourceCommand* cmd = (RenderResourceAllocator::ResourceCommand*)cur;
				cur += sizeof(RenderResourceAllocator::ResourceCommand);

				ReleaseHardwareBuffer(RenderResource::VERTEX_BUFFER, cmd->resource_handle);
				break;
			}

			case RenderResourceAllocator::ALLOCATE_INDEX_BUFFER:
			{
				RenderResourceAllocator::AllocateIndexBufferCmd* cmd = (RenderResourceAllocator::AllocateIndexBufferCmd*)cur;
				cur += sizeof(RenderResourceAllocator::AllocateIndexBufferCmd);

				AllocateIndexBuffer(cmd->resource_handle, cmd->desc, cmd->data);
				break;
			}
			case RenderResourceAllocator::RELEASE_INDEX_BUFFER:
			{
				RenderResourceAllocator::ResourceCommand* cmd = (RenderResourceAllocator::ResourceCommand*)cur;
				cur += sizeof(RenderResourceAllocator::ResourceCommand);

				ReleaseHardwareBuffer(RenderResource::INDEX_BUFFER, cmd->resource_handle);
				break;
			}

			case RenderResourceAllocator::ALLOCATE_CONSTANT_BUFFER:
			{
				RenderResourceAllocator::AllocateConstantBufferCmd* cmd = (RenderResourceAllocator::AllocateConstantBufferCmd*)cur;
				cur += sizeof(RenderResourceAllocator::AllocateConstantBufferCmd);

				AllocateConstantBuffer(cmd->resource_handle, cmd->size, cmd->data);
				break;
			}
			case RenderResourceAllocator::RELEASE_CONSTANT_BUFFER:
			{
				RenderResourceAllocator::ResourceCommand* cmd = (RenderResourceAllocator::ResourceCommand*)cur;
				cur += sizeof(RenderResourceAllocator::ResourceCommand);

				ReleaseHardwareBuffer(RenderResource::CONSTANT_BUFFER, cmd->resource_handle);
				break;
			}

			case RenderResourceAllocator::ALLOCATE_SHADER:
			{
				RenderResourceAllocator::AllocateShaderCmd* cmd = (RenderResourceAllocator::AllocateShaderCmd*)cur;
				cur += sizeof(RenderResourceAllocator::AllocateShaderCmd);
				AllocateShader(cmd->resource_handle, cmd->platform_data_size, cmd->platform_data);
				break;
			}
			case RenderResourceAllocator::RELEASE_SHADER:
			{
				RenderResourceAllocator::ResourceCommand* cmd = (RenderResourceAllocator::ResourceCommand*)cur;
				cur += sizeof(RenderResourceAllocator::ResourceCommand);

				ReleaseShader(cmd->resource_handle);
				break;
			}

			case RenderResourceAllocator::ALLOCATE_VERTEX_DECLARATION:
			{
				RenderResourceAllocator::AllocateVertexDeclarationCmd* cmd = (RenderResourceAllocator::AllocateVertexDeclarationCmd*)cur;
				cur += sizeof(RenderResourceAllocator::AllocateVertexDeclarationCmd);
				AllocateVertexDeclaration(cmd->vertex_declaration);
				break;
			}

			case RenderResourceAllocator::RELEASE_VERTEX_DECLARATION:
			{
				RenderResourceAllocator::ResourceCommand* cmd = (RenderResourceAllocator::ResourceCommand*)cur;
				cur += sizeof(RenderResourceAllocator::ResourceCommand);

				ReleaseVertexDeclaration(cmd->resource_handle);
				break;
			}

			case RenderResourceAllocator::ALLOCATE_RENDER_TARGET:
			{
				RenderResourceAllocator::AllocateRenderTargetCmd* cmd = (RenderResourceAllocator::AllocateRenderTargetCmd*)cur;
				cur += sizeof(RenderResourceAllocator::AllocateRenderTargetCmd);
				AllocateRenderTarget(cmd->resource_handle, cmd->desc, cmd->bind_flags);
				break;
			}
			case RenderResourceAllocator::RELEASE_RENDER_TARGET:
			{
				RenderResourceAllocator::ResourceCommand* cmd = (RenderResourceAllocator::ResourceCommand*)cur;
				cur += sizeof(RenderResourceAllocator::ResourceCommand);

				ReleaseRenderTarget(cmd->resource_handle);
				break;
			}

			case RenderResourceAllocator::ALLOCATE_TEXTURE:
			{
				RenderResourceAllocator::AllocateTextureCmd* cmd = (RenderResourceAllocator::AllocateTextureCmd*)cur;
				cur += sizeof(RenderResourceAllocator::AllocateTextureCmd);

				AllocateTexture(cmd->resource_handle, cmd->desc, cmd->surface_data);
				break;
			}
			case RenderResourceAllocator::RELEASE_TEXTURE:
			{
				RenderResourceAllocator::ResourceCommand* cmd = (RenderResourceAllocator::ResourceCommand*)cur;
				cur += sizeof(RenderResourceAllocator::ResourceCommand);

				ReleaseTexture(cmd->resource_handle);
				break;
			}

			case RenderResourceAllocator::ALLOCATE_RAW_BUFFER:
			{
				RenderResourceAllocator::AllocateRawBufferCmd* cmd = (RenderResourceAllocator::AllocateRawBufferCmd*)cur;
				cur += sizeof(RenderResourceAllocator::AllocateRawBufferCmd);

				AllocateRawBuffer(cmd->resource_handle, cmd->desc, cmd->data);
				break;
			}
			case RenderResourceAllocator::RELEASE_RAW_BUFFER:
			{
				RenderResourceAllocator::ResourceCommand* cmd = (RenderResourceAllocator::ResourceCommand*)cur;
				cur += sizeof(RenderResourceAllocator::ResourceCommand);

				ReleaseHardwareBuffer(RenderResource::RAW_BUFFER, cmd->resource_handle);
				break;
			}

			default:
				AssertMsg(false, "Invalid resource command");

			};
		}

		resource_allocator->Clear();
		resource_allocator->Unlock();
	}
	//-------------------------------------------------------------------------------
	void D3D11ResourceManager::ReleaseResource(const RenderResource& resource)
	{
		AssertMsg(IsValid(resource.GetHandle()), "Resource doesn't have a valid handle");
		RenderResource::Type type = resource.GetType();
		switch (type)
		{
		case RenderResource::RENDER_TARGET:
			ReleaseRenderTarget(resource.GetHandle());
			break;
		case RenderResource::TEXTURE:
			ReleaseTexture(resource.GetHandle());
			break;
		case RenderResource::CONSTANT_BUFFER:
		case RenderResource::VERTEX_BUFFER:
		case RenderResource::INDEX_BUFFER:
		case RenderResource::RAW_BUFFER:
			ReleaseHardwareBuffer(type, resource.GetHandle());
			break;
		case RenderResource::VERTEX_DECLARATION:
			ReleaseVertexDeclaration(resource.GetHandle());
			break;
		default:
			AssertMsg(false, "Invalid resource type");
		};

		_handle_lut[resource.GetHandle()] = Invalid<uint32_t>();
	}
	//-------------------------------------------------------------------------------
	D3D11RenderTarget* D3D11ResourceManager::GetRenderTarget(uint32_t handle)
	{
		Assert(IsValid(handle));
		Assert(_handle_lut.size() > handle);
		return _render_target_pool.GetObject(GetIndex(_handle_lut[handle], RenderResource::RENDER_TARGET));
	}
	D3D11VertexBuffer* D3D11ResourceManager::GetVertexBuffer(uint32_t handle)
	{
		Assert(IsValid(handle));
		Assert(_handle_lut.size() > handle);
		return _vertex_buffer_pool.GetObject(GetIndex(_handle_lut[handle], RenderResource::VERTEX_BUFFER));
	}
	D3D11IndexBuffer* D3D11ResourceManager::GetIndexBuffer(uint32_t handle)
	{
		Assert(IsValid(handle));
		Assert(_handle_lut.size() > handle);
		return _index_buffer_pool.GetObject(GetIndex(_handle_lut[handle], RenderResource::INDEX_BUFFER));
	}
	D3D11ConstantBuffer* D3D11ResourceManager::GetConstantBuffer(uint32_t handle)
	{
		Assert(IsValid(handle));
		Assert(_handle_lut.size() > handle);
		return _constant_buffer_pool.GetObject(GetIndex(_handle_lut[handle], RenderResource::CONSTANT_BUFFER));
	}
	D3D11VertexDeclaration* D3D11ResourceManager::GetVertexDeclaration(uint32_t handle)
	{
		Assert(IsValid(handle));
		Assert(_handle_lut.size() > handle);
		return _vertex_declaration_pool.GetObject(GetIndex(_handle_lut[handle], RenderResource::VERTEX_DECLARATION));
	}
	D3D11Shader* D3D11ResourceManager::GetShader(uint32_t handle)
	{
		Assert(IsValid(handle));
		Assert(_handle_lut.size() > handle);
		return _shader_pool.GetObject(GetIndex(_handle_lut[handle], RenderResource::SHADER));
	}

	ID3D11BlendState* D3D11ResourceManager::GetBlendState(const D3D11_BLEND_DESC& desc)
	{
		BlendStateMap::iterator it = _blend_states.find(desc);
		if (it != _blend_states.end())
		{
			return it->second.Get();
		}

		ID3D11BlendState* state;
		_device->GetD3DDevice()->CreateBlendState(&desc, &state);
		_blend_states[desc] = state;
		state->Release();

		return state;
	}
	ID3D11DepthStencilState* D3D11ResourceManager::GetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& desc)
	{
		DepthStencilStateMap::iterator it = _depth_stencil_states.find(desc);
		if (it != _depth_stencil_states.end())
		{
			return it->second.Get();
		}

		ID3D11DepthStencilState* state;
		_device->GetD3DDevice()->CreateDepthStencilState(&desc, &state);
		_depth_stencil_states[desc] = state;
		state->Release();

		return state;
	}
	ID3D11RasterizerState* D3D11ResourceManager::GetRasterizerState(const D3D11_RASTERIZER_DESC& desc)
	{
		RasterizerStateMap::iterator it = _rasterizer_states.find(desc);
		if (it != _rasterizer_states.end())
		{
			return it->second.Get();
		}

		ID3D11RasterizerState* state;
		_device->GetD3DDevice()->CreateRasterizerState(&desc, &state);
		_rasterizer_states[desc] = state;
		state->Release();

		return state;
	}
	ID3D11SamplerState* D3D11ResourceManager::GetSamplerState(const D3D11_SAMPLER_DESC& desc)
	{
		SamplerStateMap::iterator it = _sampler_states.find(desc);
		if (it != _sampler_states.end())
		{
			return it->second.Get();
		}

		ID3D11SamplerState* state;
		D3D_VERIFY(_device->GetD3DDevice()->CreateSamplerState(&desc, &state));
		_sampler_states[desc] = state;
		state->Release();

		return state;
	}


	ID3D11InputLayout* D3D11ResourceManager::GetInputLayout(uint32_t vertex_decl_handle, D3D11ShaderProgram<ID3D11VertexShader>* vertex_program)
	{
		Assert(vertex_program);
		Assert(IsValid(vertex_decl_handle));
		ScopedLock<CriticalSection> _lock(_input_layout_lock); // Used to avoid race conditions if multiple render contexts are trying to access the same input layout at once

		D3D11VertexDeclaration* vertex_decl = GetVertexDeclaration(vertex_decl_handle);
		map<D3D11ShaderProgram<ID3D11VertexShader>*, ID3D11InputLayout*>::iterator it = vertex_decl->input_layouts.find(vertex_program);
		if (it != vertex_decl->input_layouts.end())
		{
			return it->second;
		}

		// No matching input layout found so we have to create a new one
		ComPtr<ID3D11Device> device = _device->GetD3DDevice();
		ID3D11InputLayout* il = NULL;
		// TODO: Share input-layouts between vertex declarations with the same element descriptions
		D3D_VERIFY(device->CreateInputLayout(vertex_decl->elements.data(), 
			(uint32_t)vertex_decl->elements.size(),
			vertex_program->GetByteCode()->data(), 
			(uint32_t)vertex_program->GetByteCode()->size(), 
			&il));

		vertex_decl->input_layouts[vertex_program] = il;
		return il;
	}
	ID3D11ShaderResourceView* D3D11ResourceManager::GetResourceView(uint32_t handle, bool srgb)
	{
		Assert(IsValid(handle));
		uint16_t type = _handle_lut[handle] >> 16;
		uint32_t index = _handle_lut[handle] & 0xffff;

		switch (type)
		{
		case RenderResource::RENDER_TARGET:
		{
			D3D11RenderTarget* rt = _render_target_pool.GetObject(index);
			return rt->srv.Get();
		}
		case RenderResource::TEXTURE:
		{
			D3D11Texture* tex = _texture_pool.GetObject(index);
			if (srgb)
				return tex->srv_srgb.Get();
			else
				return tex->srv.Get();
		}
		case RenderResource::RAW_BUFFER:
		{
			D3D11RawBuffer* buf = _raw_buffer_pool.GetObject(index);
			return buf->srv.Get();
		}
		default:
			Assert(false);
			break;
		};

		return NULL;
	}

	ID3D11UnorderedAccessView* D3D11ResourceManager::GetUnorderedAccessView(uint32_t handle)
	{
		Assert(IsValid(handle));
		uint16_t type = _handle_lut[handle] >> 16;
		uint32_t index = _handle_lut[handle] & 0xffff;

		switch (type)
		{
		case RenderResource::RENDER_TARGET:
		{
			D3D11RenderTarget* rt = _render_target_pool.GetObject(index);
			Assert(rt->uav.Get()); // Resource doesn't have any unordered access view.
			return rt->uav.Get();
		}
			break;
		default:
			Assert(false);
			break;
		};

		return NULL;
	}

	//-------------------------------------------------------------------------------

	void D3D11ResourceManager::AllocateRenderTarget(uint32_t handle, const TextureDesc& desc, uint32_t bind_flags)
	{
		Assert(IsValid(handle));
		if (_handle_lut.size() <= handle)
		{
			_handle_lut.insert(_handle_lut.end(), (handle - _handle_lut.size() + 1), Invalid<uint32_t>());
		}

		D3D11RenderTarget* result = new (_render_target_pool.Allocate()) D3D11RenderTarget();
		_texture_manager->CreateRenderTarget(result, desc, bind_flags);

		// Store the index in the lookup-table
		_handle_lut[handle] = BuildHandle(RenderResource::RENDER_TARGET, _render_target_pool.GetIndex(result));

		_num_allocations++;
	}
	void D3D11ResourceManager::AllocateBackBuffer(uint32_t handle, const TextureDesc& desc, IDXGISwapChain* swap_chain)
	{
		Assert(IsValid(handle));
		if (_handle_lut.size() <= handle)
		{
			_handle_lut.insert(_handle_lut.end(), (handle - _handle_lut.size() + 1), Invalid<uint32_t>());
		}

		D3D11RenderTarget* result = new (_render_target_pool.Allocate()) D3D11RenderTarget();
		_texture_manager->CreateBackBuffer(result, swap_chain, desc);

		// Store the index in the lookup-table
		_handle_lut[handle] = BuildHandle(RenderResource::RENDER_TARGET, _render_target_pool.GetIndex(result));

		_num_allocations++;
	}
	void D3D11ResourceManager::ReleaseRenderTarget(uint32_t handle)
	{
		Assert(IsValid(handle));
		Assert(_handle_lut.size() > handle);

		uint32_t idx = GetIndex(_handle_lut[handle], RenderResource::RENDER_TARGET);
		D3D11RenderTarget* render_target = _render_target_pool.GetObject(idx);
		_texture_manager->DestroyRenderTarget(render_target);

		render_target->~D3D11RenderTarget();
		_render_target_pool.Release(render_target);
		_handle_lut[handle] = Invalid<uint32_t>();
		_num_allocations--;
	}

	//-------------------------------------------------------------------------------

	void D3D11ResourceManager::AllocateTexture(uint32_t handle, const TextureDesc& desc, uint8_t** surface_data)
	{
		Assert(IsValid(handle));
		if (_handle_lut.size() <= handle)
		{
			_handle_lut.insert(_handle_lut.end(), (handle - _handle_lut.size() + 1), Invalid<uint32_t>());
		}

		D3D11Texture* result = new (_texture_pool.Allocate()) D3D11Texture;
		_texture_manager->CreateTexture(result, desc, surface_data);

		// Store the index in the lookup-table
		_handle_lut[handle] = BuildHandle(RenderResource::TEXTURE, _texture_pool.GetIndex(result));

		_num_allocations++;
	}
	void D3D11ResourceManager::ReleaseTexture(uint32_t handle)
	{
		Assert(IsValid(handle));
		Assert(_handle_lut.size() > handle);

		uint32_t idx = GetIndex(_handle_lut[handle], RenderResource::TEXTURE);
		D3D11Texture* texture = _texture_pool.GetObject(idx);
		_texture_manager->DestroyTexture(texture);

		texture->~D3D11Texture();
		_texture_pool.Release(texture);
		_handle_lut[handle] = Invalid<uint32_t>();
		_num_allocations--;
	}

	//-------------------------------------------------------------------------------
	void D3D11ResourceManager::AllocateShader(uint32_t handle, uint32_t platform_data_size, void* platform_data)
	{
		Assert(IsValid(handle));
		if (_handle_lut.size() <= handle)
		{
			_handle_lut.insert(_handle_lut.end(), (handle - _handle_lut.size() + 1), Invalid<uint32_t>());
		}

		D3D11Shader* d3d_shader = new (_shader_pool.Allocate()) D3D11Shader();
		Assert(d3d_shader);
		d3d_shader->Load(this, platform_data, platform_data_size);


		_handle_lut[handle] = BuildHandle(RenderResource::SHADER, _shader_pool.GetIndex(d3d_shader));
		_num_allocations++;
	}
	void D3D11ResourceManager::ReleaseShader(uint32_t handle)
	{
		Assert(IsValid(handle));
		Assert(_handle_lut.size() > handle);

		uint32_t idx = GetIndex(_handle_lut[handle], RenderResource::SHADER);
		D3D11Shader* d3d_shader = _shader_pool.GetObject(idx);
		d3d_shader->~D3D11Shader();

		_shader_pool.Release(d3d_shader);
		_handle_lut[handle] = Invalid<uint32_t>();
		_num_allocations--;
	}

	//-------------------------------------------------------------------------------

	void D3D11ResourceManager::AllocateVertexDeclaration(const RVertexDeclaration& decl)
	{
		Assert(IsValid(decl.GetHandle()));
		if (_handle_lut.size() <= decl.GetHandle())
		{
			_handle_lut.insert(_handle_lut.end(), (decl.GetHandle() - _handle_lut.size() + 1), Invalid<uint32_t>());
		}

		D3D11VertexDeclaration* d3d_decl = new (_vertex_declaration_pool.Allocate()) D3D11VertexDeclaration();
		Assert(d3d_decl);
		d3d_decl->elements.reserve(decl.Count());
		for (uint32_t i = 0; i < decl.Count(); ++i)
		{
			const RVertexDeclaration::Element& elem = decl.GetElement(i);
			D3D11_INPUT_ELEMENT_DESC desc;
			desc.Format = vertex_declaration_helper::element_type_to_dxgi_format[elem.type];
			desc.InputSlot = elem.slot;
			desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = 0;
			desc.SemanticIndex = elem.sem_index;
			desc.SemanticName = vertex_declaration_helper::element_semantic_to_string_table[elem.semantic];
			desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

			d3d_decl->elements.push_back(desc);
		}

		_handle_lut[decl.GetHandle()] = BuildHandle(RenderResource::VERTEX_DECLARATION, _vertex_declaration_pool.GetIndex(d3d_decl));
		_num_allocations++;
	}
	void D3D11ResourceManager::ReleaseVertexDeclaration(uint32_t handle)
	{
		Assert(IsValid(handle));
		Assert(_handle_lut.size() > handle);

		uint32_t idx = GetIndex(_handle_lut[handle], RenderResource::VERTEX_DECLARATION);
		D3D11VertexDeclaration* d3d_decl = _vertex_declaration_pool.GetObject(idx);

		// Release input layouts
		for (auto& il : d3d_decl->input_layouts)
		{
			il.second->Release();
		}

		d3d_decl->elements.clear();
		d3d_decl->input_layouts.clear();

		d3d_decl->~D3D11VertexDeclaration();
		_vertex_declaration_pool.Release(d3d_decl);
		_handle_lut[handle] = Invalid<uint32_t>();
		_num_allocations--;
	}

	//-------------------------------------------------------------------------------

	void D3D11ResourceManager::AllocateVertexBuffer(uint32_t handle, const VertexBufferDesc& desc, void* initial_data)
	{
		Assert(IsValid(handle));
		if (_handle_lut.size() <= handle)
		{
			_handle_lut.insert(_handle_lut.end(), (handle - _handle_lut.size() + 1), Invalid<uint32_t>());
		}

		D3D11VertexBuffer* d3d_buffer = new (_vertex_buffer_pool.Allocate()) D3D11VertexBuffer();
		_hw_buffer_manager->CreateVertexBuffer(d3d_buffer, desc, initial_data);

		_handle_lut[handle] = BuildHandle(RenderResource::VERTEX_BUFFER, _vertex_buffer_pool.GetIndex(d3d_buffer));
		_num_allocations++;
	}
	void D3D11ResourceManager::AllocateIndexBuffer(uint32_t handle, const IndexBufferDesc& desc, void* initial_data)
	{
		Assert(IsValid(handle));
		if (_handle_lut.size() <= handle)
		{
			_handle_lut.insert(_handle_lut.end(), (handle - _handle_lut.size() + 1), Invalid<uint32_t>());
		}

		D3D11IndexBuffer* d3d_buffer = new (_index_buffer_pool.Allocate()) D3D11IndexBuffer();
		_hw_buffer_manager->CreateIndexBuffer(d3d_buffer, desc, initial_data);

		_handle_lut[handle] = BuildHandle(RenderResource::INDEX_BUFFER, _index_buffer_pool.GetIndex(d3d_buffer));
		_num_allocations++;
	}
	void D3D11ResourceManager::AllocateConstantBuffer(uint32_t handle, uint32_t size, void* data)
	{
		Assert(IsValid(handle));
		if (_handle_lut.size() <= handle)
		{
			_handle_lut.insert(_handle_lut.end(), (handle - _handle_lut.size() + 1), Invalid<uint32_t>());
		}

		D3D11ConstantBuffer* d3d_buffer = new (_constant_buffer_pool.Allocate()) D3D11ConstantBuffer();
		_hw_buffer_manager->CreateConstantBuffer(d3d_buffer, size, data);

		_handle_lut[handle] = BuildHandle(RenderResource::CONSTANT_BUFFER, _constant_buffer_pool.GetIndex(d3d_buffer));
		_num_allocations++;
	}
	void D3D11ResourceManager::AllocateRawBuffer(uint32_t handle, const RawBufferDesc& desc, void* initial_data)
	{
		Assert(IsValid(handle));
		if (_handle_lut.size() <= handle)
		{
			_handle_lut.insert(_handle_lut.end(), (handle - _handle_lut.size() + 1), Invalid<uint32_t>());
		}

		D3D11RawBuffer* d3d_buffer = new (_raw_buffer_pool.Allocate()) D3D11RawBuffer();
		_hw_buffer_manager->CreateRawBuffer(d3d_buffer, desc, initial_data);

		_handle_lut[handle] = BuildHandle(RenderResource::RAW_BUFFER, _raw_buffer_pool.GetIndex(d3d_buffer));
		_num_allocations++;
	}

	void D3D11ResourceManager::ReleaseHardwareBuffer(uint32_t type, uint32_t handle)
	{
		Assert(IsValid(handle));
		Assert(_handle_lut.size() > handle);

		switch (type)
		{
		case RenderResource::VERTEX_BUFFER:
		{
			D3D11VertexBuffer* d3d_buffer = _vertex_buffer_pool.GetObject(GetIndex(_handle_lut[handle],
				RenderResource::VERTEX_BUFFER));
			_hw_buffer_manager->DestroyVertexBuffer(d3d_buffer);

			d3d_buffer->~D3D11VertexBuffer();
			_vertex_buffer_pool.Release(d3d_buffer);
		}
			break;
		case RenderResource::INDEX_BUFFER:
		{
			D3D11IndexBuffer* d3d_buffer = _index_buffer_pool.GetObject(GetIndex(_handle_lut[handle],
				RenderResource::INDEX_BUFFER));
			_hw_buffer_manager->DestroyIndexBuffer(d3d_buffer);

			d3d_buffer->~D3D11IndexBuffer();
			_index_buffer_pool.Release(d3d_buffer);
		}
			break;
		case RenderResource::CONSTANT_BUFFER:
		{
			D3D11ConstantBuffer* d3d_buffer = _constant_buffer_pool.GetObject(GetIndex(_handle_lut[handle],
				RenderResource::CONSTANT_BUFFER));
			_hw_buffer_manager->DestroyConstantBuffer(d3d_buffer);

			d3d_buffer->~D3D11ConstantBuffer();
			_constant_buffer_pool.Release(d3d_buffer);
		}
			break;
		case RenderResource::RAW_BUFFER:
		{
			D3D11RawBuffer* d3d_buffer = _raw_buffer_pool.GetObject(GetIndex(_handle_lut[handle],
				RenderResource::RAW_BUFFER));
			_hw_buffer_manager->DestroyRawBuffer(d3d_buffer);

			d3d_buffer->~D3D11RawBuffer();
			_raw_buffer_pool.Release(d3d_buffer);
		}
			break;
		};
		_handle_lut[handle] = Invalid<uint32_t>();
		_num_allocations--;
	}

	void D3D11ResourceManager::UpdateResource(D3D11DeviceContext* context, RenderResource* resource, void* resource_data)
	{
		Assert(IsValid(resource->GetHandle()));
		Assert(_handle_lut.size() > resource->GetHandle());

		switch (resource->GetType())
		{
		case RenderResource::VERTEX_BUFFER:
		{
			D3D11VertexBuffer* d3d_buffer = _vertex_buffer_pool.GetObject(GetIndex(_handle_lut[resource->GetHandle()],
				RenderResource::VERTEX_BUFFER));
			_hw_buffer_manager->UpdateVertexBuffer(context, d3d_buffer, resource_data);
		}
			break;
		case RenderResource::INDEX_BUFFER:
		{
			D3D11IndexBuffer* d3d_buffer = _index_buffer_pool.GetObject(GetIndex(_handle_lut[resource->GetHandle()],
				RenderResource::INDEX_BUFFER));
			_hw_buffer_manager->UpdateIndexBuffer(context, d3d_buffer, resource_data);

		}
			break;
		case RenderResource::CONSTANT_BUFFER:
		{
			D3D11ConstantBuffer* d3d_buffer = _constant_buffer_pool.GetObject(GetIndex(_handle_lut[resource->GetHandle()],
				RenderResource::CONSTANT_BUFFER));
			_hw_buffer_manager->UpdateConstantBuffer(context, d3d_buffer, resource_data);

		}
			break;
		case RenderResource::RAW_BUFFER:
		{
			D3D11RawBuffer* d3d_buffer = _raw_buffer_pool.GetObject(GetIndex(_handle_lut[resource->GetHandle()],
				RenderResource::RAW_BUFFER));
			_hw_buffer_manager->UpdateRawBuffer(context, d3d_buffer, resource_data);

		}
			break;
		default:
			AssertMsg(false, "Update not implemented for resource type.");
		};
	}

	//-------------------------------------------------------------------------------
	D3D11ShaderProgramManager* D3D11ResourceManager::GetShaderProgramManager()
	{
		return _shader_program_manager;
	}
	D3D11HardwareBufferManager* D3D11ResourceManager::GetHardwareBufferManager()
	{
		return _hw_buffer_manager;
	}
	D3D11MemoryStatistics* D3D11ResourceManager::GetMemoryStatistics()
	{
		return &_memory_statistics;
	}
	//-------------------------------------------------------------------------------

} // namespace sb


