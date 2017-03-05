// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "D3D11ShaderStage.h"
#include "D3D11DeviceContext.h"
#include "D3D11Shader.h"
#include "D3D11Buffer.h"
#include "D3D11ResourceManager.h"
#include "D3D11HardwareBufferManager.h"



namespace sb
{

	D3D11ShaderStage::D3D11ShaderStage(D3D11DeviceContext* context)
		: _context(context),
		_shader(NULL),
		_shader_pass(NULL)
	{
		_resource_manager = _context->GetResourceManager();
	}
	D3D11ShaderStage::~D3D11ShaderStage()
	{

	}

	void D3D11ShaderStage::BindShader(D3D11Shader* shader, uint64_t sort_key)
	{
		if (shader != NULL)
		{
			_shader = shader;
			uint64_t shader_pass = sort_key << (64 - render_sorting::SHADER_PASS_BIT - render_sorting::SHADER_PASS_NUM_BITS);
			shader_pass = shader_pass >> (64 - render_sorting::SHADER_PASS_NUM_BITS);

			if (_shader_pass == shader->GetShaderPass((uint32_t)shader_pass))
				return;

			_shader_pass = shader->GetShaderPass((uint32_t)shader_pass);
		}
		else
		{
			_shader = NULL;
			_shader_pass = NULL;
		}

		ID3D11VertexShader* vs = NULL;
		ID3D11HullShader* hs = NULL;
		ID3D11DomainShader* ds = NULL;
		ID3D11GeometryShader* gs = NULL;
		ID3D11PixelShader* ps = NULL;
		ID3D11ComputeShader* cs = NULL;

		if (_shader_pass)
		{
			if (_shader_pass->GetVertexShader())
				vs = _shader_pass->GetVertexShader()->GetShader();

			if (_shader_pass->GetHullShader())
				hs = _shader_pass->GetHullShader()->GetShader();

			if (_shader_pass->GetDomainShader())
				ds = _shader_pass->GetDomainShader()->GetShader();

			if (_shader_pass->GetGeometryShader())
				gs = _shader_pass->GetGeometryShader()->GetShader();

			if (_shader_pass->GetPixelShader())
				ps = _shader_pass->GetPixelShader()->GetShader();

			if (_shader_pass->GetComputeShader())
				cs = _shader_pass->GetComputeShader()->GetShader();

		}

		ID3D11DeviceContext* d3dcontext = _context->GetD3DContext();
		if (_vertex_shader != vs)
		{
			_vertex_shader = vs;
			d3dcontext->VSSetShader(vs, 0, 0);
		}
		if (_hull_shader != hs)
		{
			_hull_shader = hs;
			d3dcontext->HSSetShader(hs, 0, 0);
		}
		if (_domain_shader != ds)
		{
			_domain_shader = ds;
			d3dcontext->DSSetShader(ds, 0, 0);
		}
		if (_geometry_shader != gs)
		{
			_geometry_shader = gs;
			d3dcontext->GSSetShader(gs, 0, 0);
		}
		if (_pixel_shader != ps)
		{
			_pixel_shader = ps;
			d3dcontext->PSSetShader(ps, 0, 0);
		}
		if (_compute_shader != cs)
		{
			_compute_shader = cs;
			d3dcontext->CSSetShader(cs, 0, 0);
		}



	}

	bool D3D11ShaderStage::BindProgramResources(RenderResource* resources, uint32_t resource_count,
		const vector<D3D11ShaderPassData::ShaderProgram::ResourceBindInfo>& bind_info,
		vector<ID3D11ShaderResourceView*>& resource_views, uint32_t& start, uint32_t& count)
	{
		bool changed = false;
		start = (uint32_t)-1;
		count = 0;
		uint32_t stop = 0;

		for (auto& bind : bind_info)
		{
			// Skip UAV binds as we only bind resource views here.
			if (bind.flags & D3D11ShaderPassData::TEXTURE_UAV)
				continue;

			Verify(bind.index < resource_count);
			if (resource_views.size() < (bind.bind_point + bind.bind_count + 1))
				resource_views.insert(resource_views.end(), (bind.bind_point + bind.bind_count + 1) - resource_views.size(), NULL);

			ID3D11ShaderResourceView* view = NULL;
			uint32_t handle = resources[bind.index].GetHandle();

			bool srbg = false;
			if (bind.flags & D3D11ShaderPassData::TEXTURE_SRGB)
				srbg = true;
			if (IsValid(handle))
				view = _resource_manager->GetResourceView(resources[bind.index].GetHandle(), srbg);

			start = Min(start, bind.bind_point);
			stop = Max(stop, bind.bind_point + bind.bind_count);
			if (resource_views[bind.bind_point] != view)
			{
				changed = true;
				resource_views[bind.bind_point] = view;
			}
		}
		count = stop - start;
		return changed;
	}

	bool D3D11ShaderStage::BindProgramSamplers(const vector<D3D11ShaderPassData::ShaderProgram::SamplerBindInfo>& bind_info,
		vector<ID3D11SamplerState*>& sampler_states, uint32_t& start, uint32_t& count)
	{
		bool changed = false;
		start = (uint32_t)-1;
		count = 0;

		for (auto& bind : bind_info)
		{
			if (sampler_states.size() < (bind.bind_point + bind.bind_count + 1))
				sampler_states.insert(sampler_states.end(), (bind.bind_point + bind.bind_count + 1) - sampler_states.size(), NULL);

			D3D11SamplerState* sampler_state = _shader_pass->GetSamplerState(bind.name);
			if (sampler_state)
			{
				ID3D11SamplerState* state = _shader_pass->GetSamplerState(bind.name)->GetState();

				start = Min(start, bind.bind_point);
				count += bind.bind_count;
				if (sampler_states[bind.bind_point] != state)
				{
					changed = true;
					sampler_states[bind.bind_point] = state;
				}
			}
			else
			{
				Assert(false);
				logging::Warning("Sampler state '0x%x' not found.", bind.name);
			}
		}
		return changed;
	}

	void D3D11ShaderStage::BindResources(RenderResource* resources, uint32_t resource_count)
	{
		if (!_shader_pass)
			return;

		D3D11ShaderPassData* pass_data = _shader_pass->GetData();

		uint32_t start, count;
		ID3D11DeviceContext* context = _context->GetD3DContext();
		if (_shader_pass->GetVertexShader())
		{
			if (BindProgramResources(resources, resource_count, pass_data->vertex_shader.resource_bind_info, _vs_resources, start, count))
			{
				context->VSSetShaderResources(start, count, _vs_resources.data() + start);
			}
			if (BindProgramSamplers(pass_data->vertex_shader.sampler_bind_info, _vs_sampler_states, start, count))
			{
				context->VSSetSamplers(start, count, _vs_sampler_states.data() + start);
			}
		}
		if (_shader_pass->GetHullShader())
		{
			if (BindProgramResources(resources, resource_count, pass_data->hull_shader.resource_bind_info, _hs_resources, start, count))
			{
				context->HSSetShaderResources(start, count, _hs_resources.data() + start);
			}
			if (BindProgramSamplers(pass_data->hull_shader.sampler_bind_info, _hs_sampler_states, start, count))
			{
				context->HSSetSamplers(start, count, _hs_sampler_states.data() + start);
			}
		}
		if (_shader_pass->GetDomainShader())
		{
			if (BindProgramResources(resources, resource_count, pass_data->domain_shader.resource_bind_info, _ds_resources, start, count))
			{
				context->DSSetShaderResources(start, count, _ds_resources.data() + start);
			}
			if (BindProgramSamplers(pass_data->domain_shader.sampler_bind_info, _ds_sampler_states, start, count))
			{
				context->DSSetSamplers(start, count, _ds_sampler_states.data() + start);
			}
		}
		if (_shader_pass->GetGeometryShader())
		{
			if (BindProgramResources(resources, resource_count, pass_data->geometry_shader.resource_bind_info, _gs_resources, start, count))
			{
				context->GSSetShaderResources(start, count, _gs_resources.data() + start);
			}
			if (BindProgramSamplers(pass_data->geometry_shader.sampler_bind_info, _gs_sampler_states, start, count))
			{
				context->GSSetSamplers(start, count, _gs_sampler_states.data() + start);
			}
		}
		if (_shader_pass->GetPixelShader())
		{
			if (BindProgramResources(resources, resource_count, pass_data->pixel_shader.resource_bind_info, _ps_resources, start, count))
			{
				context->PSSetShaderResources(start, count, _ps_resources.data() + start);
			}
			if (BindProgramSamplers(pass_data->pixel_shader.sampler_bind_info, _ps_sampler_states, start, count))
			{
				context->PSSetSamplers(start, count, _ps_sampler_states.data() + start);
			}
		}
		if (_shader_pass->GetComputeShader())
		{
			if (BindProgramResources(resources, resource_count, pass_data->compute_shader.resource_bind_info, _cs_resources, start, count))
			{
				context->CSSetShaderResources(start, count, _cs_resources.data() + start);
			}
			if (BindProgramSamplers(pass_data->compute_shader.sampler_bind_info, _cs_sampler_states, start, count))
			{
				context->CSSetSamplers(start, count, _cs_sampler_states.data() + start);
			}
		}

	}

	void D3D11ShaderStage::BindProgramConstantBuffers(const vector<D3D11ShaderPassData::ShaderProgram::ConstantBufferBindInfo>& bind_info,
		const vector<D3D11ConstantBuffer*>& d3dbuffers, vector<ID3D11Buffer*>& outbuffers)
	{
		for (auto& bind : bind_info)
		{
			if (outbuffers.size() < (bind.bind_point + bind.bind_count + 1))
				outbuffers.insert(outbuffers.end(), (bind.bind_point + bind.bind_count + 1) - outbuffers.size(), NULL);

			Assert(bind.index < d3dbuffers.size());
			D3D11ConstantBuffer* pd3dbuffer = d3dbuffers[bind.index];
			outbuffers[bind.bind_point] = pd3dbuffer->buffer.Get();

		}
	}
	void D3D11ShaderStage::BindConstantBuffers(RConstantBuffer* buffers, uint32_t buffer_count, void* constant_data)
	{
		if (!_shader_pass)
			return;

		vector<D3D11ConstantBuffer*> d3dbuffers;

		// Update buffers first
		uint32_t offset = 0;
		for (uint32_t i = 0; i < buffer_count; ++i)
		{
			D3D11ConstantBuffer* pd3dbuffer = _resource_manager->GetConstantBuffer(buffers[i].GetHandle());
			d3dbuffers.push_back(pd3dbuffer);
			if (buffers[i].GetType() == RConstantBuffer::TYPE_LOCAL)
			{
				_resource_manager->GetHardwareBufferManager()->UpdateConstantBuffer(_context, pd3dbuffer, memory::PointerAdd(constant_data, offset));
			}

			if (buffers[i].GetType() != RConstantBuffer::TYPE_GLOBAL)
			{
				offset += buffers[i].GetSize();
			}
		}

		D3D11ShaderPassData* shader_data = _shader_pass->GetData();
		ID3D11DeviceContext* context = _context->GetD3DContext();

		vector<ID3D11Buffer*> id3dbuffers;
		if (_shader_pass->GetVertexShader())
		{
			BindProgramConstantBuffers(shader_data->vertex_shader.constant_buffer_bind_info, d3dbuffers, id3dbuffers);
			if (id3dbuffers.size())
			{
				context->VSSetConstantBuffers(0, (UINT)id3dbuffers.size(), id3dbuffers.data());
			}
			id3dbuffers.clear();
		}
		if (_shader_pass->GetHullShader())
		{
			BindProgramConstantBuffers(shader_data->hull_shader.constant_buffer_bind_info, d3dbuffers, id3dbuffers);
			if (id3dbuffers.size())
			{
				context->HSSetConstantBuffers(0, (UINT)id3dbuffers.size(), id3dbuffers.data());
			}
			id3dbuffers.clear();
		}
		if (_shader_pass->GetDomainShader())
		{
			BindProgramConstantBuffers(shader_data->domain_shader.constant_buffer_bind_info, d3dbuffers, id3dbuffers);
			if (id3dbuffers.size())
			{
				context->DSSetConstantBuffers(0, (UINT)id3dbuffers.size(), id3dbuffers.data());
			}
			id3dbuffers.clear();
		}
		if (_shader_pass->GetGeometryShader())
		{
			BindProgramConstantBuffers(shader_data->geometry_shader.constant_buffer_bind_info, d3dbuffers, id3dbuffers);
			if (id3dbuffers.size())
			{
				context->GSSetConstantBuffers(0, (UINT)id3dbuffers.size(), id3dbuffers.data());
			}
			id3dbuffers.clear();
		}
		if (_shader_pass->GetPixelShader())
		{
			BindProgramConstantBuffers(shader_data->pixel_shader.constant_buffer_bind_info, d3dbuffers, id3dbuffers);
			if (id3dbuffers.size())
			{
				context->PSSetConstantBuffers(0, (UINT)id3dbuffers.size(), id3dbuffers.data());
			}
			id3dbuffers.clear();
		}
		if (_shader_pass->GetComputeShader())
		{
			BindProgramConstantBuffers(shader_data->compute_shader.constant_buffer_bind_info, d3dbuffers, id3dbuffers);
			if (id3dbuffers.size())
			{
				context->CSSetConstantBuffers(0, (UINT)id3dbuffers.size(), id3dbuffers.data());
			}
			id3dbuffers.clear();
		}


	}

	void D3D11ShaderStage::Clear()
	{
		_vs_resources.clear();
		_vs_sampler_states.clear();

		_hs_resources.clear();
		_hs_sampler_states.clear();

		_ds_resources.clear();
		_ds_sampler_states.clear();

		_gs_resources.clear();
		_gs_sampler_states.clear();

		_ps_resources.clear();
		_ps_sampler_states.clear();

		_cs_resources.clear();
		_cs_sampler_states.clear();

		_shader = NULL;
		_shader_pass = NULL;

		_vertex_shader = NULL;
		_hull_shader = NULL;
		_domain_shader = NULL;
		_geometry_shader = NULL;
		_pixel_shader = NULL;
		_compute_shader = NULL;
	}

	D3D11ShaderPass* D3D11ShaderStage::GetShaderPass()
	{
		return _shader_pass;
	}

} // namespace sb
