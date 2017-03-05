// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "D3D11Shader.h"
#include "D3D11ShaderData.h"
#include "D3D11ShaderProgram.h"
#include "D3D11ResourceManager.h"
#include "D3D11ShaderProgramManager.h"
#include "D3D11DeviceContext.h"

#include <Engine/Rendering/Shader.h>



namespace sb
{

	//-------------------------------------------------------------------------------
	namespace
	{

		/// Helper-function for creating a shader program from byte-code
		template<typename T>
		D3D11ShaderProgram<T>* CreateProgram(D3D11ShaderProgramManager* shader_manager, D3D11ShaderPassData::ShaderProgram& program_data)
		{
			if (program_data.byte_code.size())
			{
				return new D3D11ShaderProgram<T>(shader_manager->GetShader<T>(program_data.byte_code), &program_data.byte_code);
			}
			return NULL;
		}


	}

	//-------------------------------------------------------------------------------
	D3D11ShaderPass::D3D11ShaderPass()
		: _resource_manager(NULL),
		_vertex_shader(NULL),
		_hull_shader(NULL),
		_domain_shader(NULL),
		_geometry_shader(NULL),
		_pixel_shader(NULL),
		_compute_shader(NULL),
		_blend_state(NULL),
		_depth_stencil_state(NULL),
		_rasterizer_state(NULL),
		_data(NULL)
	{

	}
	D3D11ShaderPass::~D3D11ShaderPass()
	{
		// Release all programs
		D3D11ShaderProgramManager* shader_manager = _resource_manager->GetShaderProgramManager();

		if (_vertex_shader)
		{
			shader_manager->ReleaseShader(_vertex_shader->GetShader());
			delete _vertex_shader;
		}
		if (_hull_shader)
		{
			shader_manager->ReleaseShader(_hull_shader->GetShader());
			delete _hull_shader;
		}
		if (_domain_shader)
		{
			shader_manager->ReleaseShader(_domain_shader->GetShader());
			delete _domain_shader;
		}
		if (_geometry_shader)
		{
			shader_manager->ReleaseShader(_geometry_shader->GetShader());
			delete _geometry_shader;
		}
		if (_pixel_shader)
		{
			shader_manager->ReleaseShader(_pixel_shader->GetShader());
			delete _pixel_shader;
		}
		if (_compute_shader)
		{
			shader_manager->ReleaseShader(_compute_shader->GetShader());
			delete _compute_shader;
		}


		if (_blend_state)
		{
			delete _blend_state;
		}
		if (_depth_stencil_state)
		{
			delete _depth_stencil_state;
		}
		if (_rasterizer_state)
		{
			delete _rasterizer_state;
		}
		if (!_sampler_states.empty())
		{
			for (auto& state : _sampler_states)
			{
				delete state.second;
			}
			_sampler_states.clear();
		}

	}

	void D3D11ShaderPass::Load(D3D11ResourceManager* resource_manager, D3D11ShaderPassData* pass_data)
	{
		_resource_manager = resource_manager;
		_data = pass_data;

		D3D11ShaderProgramManager* shader_manager = _resource_manager->GetShaderProgramManager();

		// Create shaders

		_vertex_shader = CreateProgram<ID3D11VertexShader>(shader_manager, _data->vertex_shader);
		_hull_shader = CreateProgram<ID3D11HullShader>(shader_manager, _data->hull_shader);
		_domain_shader = CreateProgram<ID3D11DomainShader>(shader_manager, _data->domain_shader);
		_geometry_shader = CreateProgram<ID3D11GeometryShader>(shader_manager, _data->geometry_shader);
		_pixel_shader = CreateProgram<ID3D11PixelShader>(shader_manager, _data->pixel_shader);
		_compute_shader = CreateProgram<ID3D11ComputeShader>(shader_manager, _data->compute_shader);

		_blend_state = new D3D11BlendState();
		_blend_state->Create(resource_manager, _data->render_state.blend_desc, _data->render_state.blend_factor, _data->render_state.sample_mask);

		_depth_stencil_state = new D3D11DepthStencilState();
		_depth_stencil_state->Create(resource_manager, _data->render_state.depth_stencil_desc, _data->render_state.stencil_ref);

		_rasterizer_state = new D3D11RasterizerState();
		_rasterizer_state->Create(resource_manager, _data->render_state.rasterizer_desc);

		for (auto& s : _data->sampler_states)
		{
			D3D11SamplerState* state = new D3D11SamplerState();
			Assert(state);
			state->Create(resource_manager, s.desc);
			_sampler_states[s.name] = state;
		}
	}

	D3D11ShaderProgram<ID3D11VertexShader>* D3D11ShaderPass::GetVertexShader()
	{
		return _vertex_shader;
	}
	D3D11ShaderProgram<ID3D11HullShader>* D3D11ShaderPass::GetHullShader()
	{
		return _hull_shader;
	}
	D3D11ShaderProgram<ID3D11DomainShader>* D3D11ShaderPass::GetDomainShader()
	{
		return _domain_shader;
	}
	D3D11ShaderProgram<ID3D11GeometryShader>* D3D11ShaderPass::GetGeometryShader()
	{
		return _geometry_shader;
	}
	D3D11ShaderProgram<ID3D11PixelShader>* D3D11ShaderPass::GetPixelShader()
	{
		return _pixel_shader;
	}
	D3D11ShaderProgram<ID3D11ComputeShader>* D3D11ShaderPass::GetComputeShader()
	{
		return _compute_shader;
	}

	D3D11BlendState* D3D11ShaderPass::GetBlendState() const
	{
		return _blend_state;
	}
	D3D11DepthStencilState* D3D11ShaderPass::GetDepthStencilState() const
	{
		return _depth_stencil_state;
	}
	D3D11RasterizerState* D3D11ShaderPass::GetRasterizerState() const
	{
		return _rasterizer_state;
	}
	D3D11SamplerState* D3D11ShaderPass::GetSamplerState(StringId32 name) const
	{
		map<StringId32, D3D11SamplerState*>::const_iterator it = _sampler_states.find(name);
		if (it != _sampler_states.end())
			return it->second;
		return NULL;
	}

	D3D11ShaderPassData* D3D11ShaderPass::GetData()
	{
		return _data;
	}
	const D3D11ShaderPassData* D3D11ShaderPass::GetData() const
	{
		return _data;
	}

	//-------------------------------------------------------------------------------

	D3D11Shader::D3D11Shader()
		: _data(NULL)
	{
	}
	D3D11Shader::~D3D11Shader()
	{
		for (auto& pass : _passes)
		{
			delete pass;
		}
		_passes.clear();

		delete _data;
	}

	void D3D11Shader::Load(D3D11ResourceManager* resource_manager, void* platform_data, uint32_t platform_data_size)
	{
		if (_data) // already loaded
			return;

		_data = new D3D11ShaderData();

		// Parse platform data
		Assert(platform_data_size != 0);
		StaticMemoryStream stream(platform_data, platform_data_size);
		_data->Deserialize(stream);

		for (auto& p : _data->passes)
		{
			D3D11ShaderPass* pass = new D3D11ShaderPass();
			Assert(pass);
			pass->Load(resource_manager, &p);
			_passes.push_back(pass);
		}

	}

	D3D11ShaderPass* D3D11Shader::GetShaderPass(uint32_t index)
	{
		return _passes[index];
	}

	//-------------------------------------------------------------------------------

} // namespace sb

