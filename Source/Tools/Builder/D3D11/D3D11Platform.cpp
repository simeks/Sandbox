// Copyright 2008-2014 Simon Ekström

#include "Common.h"


#include <D3Dcompiler.h>
#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "dxguid.lib" )

#include "D3D11Platform.h"
#include "ShaderCompiler.h"
#include "DependencyDatabase.h"

#include <Engine/Rendering/Shader.h>
#include <Foundation/Json/Json.h>

#include "RenderState.h"

namespace sb
{

	namespace
	{
		ShaderVariable::Class StringToType(const char* type)
		{
			if (strcmp(type, "matrix4x4") == 0)
				return ShaderVariable::MATRIX4X4;
			if (strcmp(type, "vector4") == 0)
				return ShaderVariable::VECTOR4;
			if (strcmp(type, "vector3") == 0)
				return ShaderVariable::VECTOR3;
			if (strcmp(type, "vector2") == 0)
				return ShaderVariable::VECTOR2;
			if (strcmp(type, "scalar") == 0)
				return ShaderVariable::SCALAR;
			else
				return ShaderVariable::UNKNOWN; // Invalid type
		}

		/// Parses defines separated by spaces from a string.
		void ParseDefines(const char* str, vector<string>& defines)
		{
			string defines_str(str);

			size_t i = 0, j = 0;
			while ((j = defines_str.find(' ', j + 1)) != string::npos)
			{
				defines.push_back(defines_str.substr(i, j - i));
				i = j + 1;
			}
			defines.push_back(defines_str.substr(i));

		}
	};


	//-------------------------------------------------------------------------------

	class D3DInclude : public ID3DInclude
	{
	public:
		D3DInclude(ShaderCompiler::ShaderFileCache& cache, const string& base_path, const FilePath& source_file)
			: _cache(cache),
			_base_path(base_path),
			_source_file(source_file)
		{}

		STDMETHOD(Open)(THIS_ D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
		STDMETHOD(Close)(THIS_ LPCVOID pData);

	private:
		ShaderCompiler::ShaderFileCache& _cache;

		string _base_path;
		FilePath _source_file;

		map<const void*, FilePath> _data_to_path_lut;
	};

	HRESULT D3DInclude::Close(LPCVOID pData)
	{
		_data_to_path_lut.erase(pData);
		return S_OK;
	}
	HRESULT D3DInclude::Open(D3D_INCLUDE_TYPE include_type, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
	{
		string path;
		InputBuffer* buffer = NULL;

		if (include_type == D3D_INCLUDE_LOCAL)
		{
			// 1. First check if the file is in the same file as the original file
			path = _source_file.Directory();
			path += pFileName;
			buffer = _cache.Open(path.c_str());

			// 2. If there's a parent include, first chech relative to that
			if (!buffer && pParentData)
			{
				FilePath& parent_path = _data_to_path_lut[pParentData];
				path = parent_path.Directory();
				path += pFileName;

				buffer = _cache.Open(path.c_str());
			}
		}

		// 3. Check relative to base path
		if (!buffer)
		{
			path = _base_path;
			path += pFileName;
			buffer = _cache.Open(path.c_str());
		}

		// Nothing found there either, fail
		if (!buffer)
		{
			return E_FAIL;
		}

		*ppData = buffer->Ptr();
		*pBytes = (UINT)buffer->Length();

		_data_to_path_lut[buffer->Ptr()] = path;

		return S_OK;
	}

	//-------------------------------------------------------------------------------

	D3D11Platform::D3D11Platform(const string& base_path, const CompilerSystem::CompilerContext& context)
		: _context(context),
		_error(""),
		_base_path(base_path),
		_file_cache(context.asset_source)

	{
		// Set default
		_default_render_state.blend_desc = CD3D11_BLEND_DESC(D3D11_DEFAULT);
		_default_render_state.blend_desc.IndependentBlendEnable = FALSE;
		_default_render_state.sample_mask = 0xffffffff;
		for (int i = 0; i < 4; ++i)
			_default_render_state.blend_factor[i] = 1.0f;

		_default_render_state.depth_stencil_desc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
		_default_render_state.stencil_ref = 0;

		_default_render_state.rasterizer_desc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);
		_default_render_state.rasterizer_desc.FrontCounterClockwise = true;

		_default_sampler_state.desc = CD3D11_SAMPLER_DESC(D3D11_DEFAULT);
		_default_sampler_state.srgb = false;
	}
	D3D11Platform::~D3D11Platform()
	{
	}

	//-------------------------------------------------------------------------------
	void D3D11Platform::Precompile(const ShaderDatabase::Shader& shader)
	{
		// Parse render states
		const ConfigValue& render_states = shader.GetConfig()["render_states"];
		if (render_states.IsObject())
		{
			for (ConfigValue::ConstIterator it = render_states.Begin(); it != render_states.End(); ++it)
			{
				ParseRenderState(it->second, _render_states[it->first]);
			}
		}

		// Parse sampler states
		const ConfigValue& sampler_states = shader.GetConfig()["sampler_states"];
		if (sampler_states.IsObject())
		{
			for (ConfigValue::ConstIterator it = sampler_states.Begin(); it != sampler_states.End(); ++it)
			{
				ParseSamplerState(it->second, _sampler_states[it->first]);
				_sampler_states[it->first].name = it->first;
			}
		}

		// Parse default shader variable values
		const ConfigValue& variables = shader.GetConfig()["variables"];
		if (variables.IsArray())
		{
			for (uint32_t i = 0; i < variables.Size(); ++i)
			{
				const ConfigValue& var_node = variables["variables"][i];
				if (var_node["name"].IsString() && var_node["type"].IsString())
				{
					VariableValue var;
					var.var_class = StringToType(var_node["type"].AsString());

					if (var.var_class == ShaderVariable::SCALAR)
					{
						if (!shader_variable::ParseScalar(var_node["value"], var.scalar))
						{
							// Skip invalid value
							logging::Warning("Value for variable '%s' has an invalid format.", var_node["name"].AsString());
							continue;
						}
					}
					else if (var.var_class == ShaderVariable::VECTOR2)
					{
						if (!shader_variable::ParseVector2(var_node["value"], var.v2))
						{
							// Skip invalid value
							logging::Warning("Value for variable '%s' has an invalid format.", var_node["name"].AsString());
							continue;
						}
					}
					else if (var.var_class == ShaderVariable::VECTOR3)
					{
						if (!shader_variable::ParseVector3(var_node["value"], var.v3))
						{
							// Skip invalid value
							logging::Warning("Value for variable '%s' has an invalid format.", var_node["name"].AsString());
							continue;
						}
					}
					else if (var.var_class == ShaderVariable::VECTOR4)
					{
						if (!shader_variable::ParseVector4(var_node["value"], var.v4))
						{
							// Skip invalid value
							logging::Warning("Value for variable '%s' has an invalid format.", var_node["name"].AsString());
							continue;
						}
					}
					else if (var.var_class == ShaderVariable::MATRIX4X4)
					{
						if (!shader_variable::ParseMatrix4x4(var_node["value"], var.m44))
						{
							// Skip invalid value
							logging::Warning("Value for variable '%s' has an invalid format.", var_node["name"].AsString());
							continue;
						}
					}
					else
					{
						logging::Warning("Unknown variable type for variable '%s' in variable definition.", var_node["name"].AsString());
						continue;
					}

					_variables[var_node["name"].AsString()] = var;
				}
			}
		}


	}
	//-------------------------------------------------------------------------------
	bool D3D11Platform::Compile(const ShaderDatabase::Shader& shader, const vector<string>& defines, ShaderData& shader_data)
	{
		const ConfigValue& techniques = shader.GetConfig()["techniques"];
		if (!techniques.IsObject())
		{
			_error = "No techniques specified in shader source.";
			return false;
		}

		// Should the shader use instancing?
		bool instanced = false;
		for (vector<string>::const_iterator it = defines.begin(); it != defines.end(); ++it)
		{
			if (*it == "INSTANCED")
				instanced = true;
		}

		D3D11ShaderData d3d_shader_data;

		for (ConfigValue::ConstIterator technique_it = techniques.Begin(); technique_it != techniques.End(); ++technique_it)
		{
			const ConfigValue& passes = technique_it->second["passes"];
			if (!passes.IsArray())
			{
				logging::Warning("No shader passes in technique '%s', skipping.", technique_it->first.c_str());
				continue;
			}

			for (uint32_t p = 0; p < passes.Size(); ++p)
			{
				shader_data.passes.push_back(ShaderData::Pass());
				ShaderData::Pass& shader_pass_data = shader_data.passes.back();

				shader_pass_data.technique = technique_it->first;
				shader_pass_data.instanced = instanced;

				d3d_shader_data.passes.push_back(D3D11ShaderPassData());
				D3D11ShaderPassData& d3d_shader_pass_data = d3d_shader_data.passes.back();

				const ConfigValue& pass_node = passes[p];

				if (pass_node["render_state"].IsString())
				{
					map<string, D3D11ShaderPassData::RenderState>::iterator render_state_it = _render_states.find(pass_node["render_state"].AsString());
					if (render_state_it == _render_states.end())
					{
						logging::Warning("Could not find render state '%s', setting to default.", pass_node["render_state"].AsString());
						d3d_shader_pass_data.render_state = _default_render_state;
					}
					else
					{
						d3d_shader_pass_data.render_state = render_state_it->second;
					}
				}
				else
				{
					// No render state specified, set default
					d3d_shader_pass_data.render_state = _default_render_state;
				}

				if (pass_node["sampler_states"].IsArray())
				{
					for (uint32_t s = 0; s < pass_node["sampler_states"].Size(); ++s)
					{
						const ConfigValue& sampler_state = pass_node["sampler_states"][s];
						if (!sampler_state["state"].IsString() || !sampler_state["texture"].IsString())
						{
							logging::Warning("Sampler state requires both a 'state' and a 'texture' to be set.");
							continue;
						}

						_texture_to_state[sampler_state["texture"].AsString()] = sampler_state["state"].AsString();
					}
				}

				if (!CompilePass(pass_node, defines, shader_data, d3d_shader_pass_data))
					return false;

				if (instanced)
				{
					ParseInstanceData(technique_it->second["instance_data"], shader_data.instance_data_reflection, d3d_shader_pass_data.instance_data);

					// Round up instance data to make it multiples of float4
					if (d3d_shader_pass_data.instance_data.size && (d3d_shader_pass_data.instance_data.size % (sizeof(float)* 4)))
						d3d_shader_pass_data.instance_data.size += (sizeof(float)* 4) - (d3d_shader_pass_data.instance_data.size % (sizeof(float)* 4));


					uint32_t i = 0;
					for (auto& res : shader_data.resource_reflection.resources)
					{
						if (res.name == "instance_data_buffer")
						{
							d3d_shader_pass_data.instance_data.instance_data_slot = i;
							break;
						}
						++i;
					}

					if (IsInvalid(d3d_shader_pass_data.instance_data.instance_data_slot))
					{
						logging::Warning("No buffer with name \"instance_data_buffer\" defined in shader.");
					}
				}
			}

		}


		DynamicMemoryStream stream(&shader_data.platform_data);
		d3d_shader_data.Serialize(stream);

		return true;
	}

	void D3D11Platform::Cleanup(ShaderData& shader_data)
	{
		// Clear any allocated constant buffer templates
		for (auto& cbt : shader_data.constant_buffer_templates)
		{
			if (cbt.data)
			{
				memory::Free(cbt.data);
			}
		}
	}

	//-------------------------------------------------------------------------------
	const string& D3D11Platform::GetError() const
	{
		return _error;
	}

	ShaderCompiler::ShaderFileCache& D3D11Platform::GetCache()
	{
		return _file_cache;
	}
	//-------------------------------------------------------------------------------
	bool D3D11Platform::CompilePass(const ConfigValue& pass_node, const vector<string>& options, ShaderData& shader_data,
		D3D11ShaderPassData& d3d_shader_pass_data)
	{
		vector<D3D_SHADER_MACRO> macros;
		D3D_SHADER_MACRO macro;

		char* def = "1";
		for (uint32_t i = 0; i < options.size(); ++i)
		{
			macro.Name = options[i].c_str();
			macro.Definition = def;

			macros.push_back(macro);
		}

		if (pass_node["defines"].IsString())
		{
			vector<string> defines;
			ParseDefines(pass_node["defines"].AsString(), defines);

			for (uint32_t i = 0; i < options.size(); ++i)
			{
				macro.Name = options[i].c_str();
				macro.Definition = def;

				macros.push_back(macro);
			}
		}
		macro.Name = NULL;
		macro.Definition = NULL;
		macros.push_back(macro);

		UINT hlsl_flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

		if (_context.settings->hlsl_debug)
			hlsl_flags |= D3DCOMPILE_DEBUG;

		switch (_context.settings->hlsl_optimization_level)
		{
		case 0:
			hlsl_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL0;
			break;
		case 1:
			hlsl_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL1;
			break;
		case 2:
			hlsl_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL2;
			break;
		case 3:
			hlsl_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
			break;
		};


		if (pass_node["vertex_shader"].IsObject())
		{
			if (!CompileProgram(pass_node["vertex_shader"], macros, hlsl_flags, d3d_shader_pass_data.vertex_shader))
				return false;
			ReflectProgram(shader_data, d3d_shader_pass_data, d3d_shader_pass_data.vertex_shader);
		}
		if (pass_node["hull_shader"].IsObject())
		{
			if (!CompileProgram(pass_node["hull_shader"], macros, hlsl_flags, d3d_shader_pass_data.hull_shader))
				return false;
			ReflectProgram(shader_data, d3d_shader_pass_data, d3d_shader_pass_data.hull_shader);
		}
		if (pass_node["domain_shader"].IsObject())
		{
			if (!CompileProgram(pass_node["domain_shader"], macros, hlsl_flags, d3d_shader_pass_data.domain_shader))
				return false;
			ReflectProgram(shader_data, d3d_shader_pass_data, d3d_shader_pass_data.domain_shader);
		}
		if (pass_node["geometry_shader"].IsObject())
		{
			if (!CompileProgram(pass_node["geometry_shader"], macros, hlsl_flags, d3d_shader_pass_data.geometry_shader))
				return false;
			ReflectProgram(shader_data, d3d_shader_pass_data, d3d_shader_pass_data.geometry_shader);
		}
		if (pass_node["pixel_shader"].IsObject())
		{
			if (!CompileProgram(pass_node["pixel_shader"], macros, hlsl_flags, d3d_shader_pass_data.pixel_shader))
				return false;
			ReflectProgram(shader_data, d3d_shader_pass_data, d3d_shader_pass_data.pixel_shader);
		}
		if (pass_node["compute_shader"].IsObject())
		{
			if (!CompileProgram(pass_node["compute_shader"], macros, hlsl_flags, d3d_shader_pass_data.compute_shader))
				return false;
			ReflectProgram(shader_data, d3d_shader_pass_data, d3d_shader_pass_data.compute_shader);
		}


		return true;

	}
	bool D3D11Platform::CompileProgram(const ConfigValue& program_node, const vector<D3D_SHADER_MACRO>& macros, UINT hlsl_flags,
		D3D11ShaderPassData::ShaderProgram& program)
	{
		if (!program_node.IsObject())
			return false;

		if (!program_node["hlsl_file"].IsString() || !program_node["entry"].IsString() || !program_node["target"].IsString())
		{
			_error = "Shader program requires a source hlsl file, an entry-point and a target.";
			return false;
		}

		FilePath file_path;
		file_path = _base_path;
		file_path += program_node["hlsl_file"].AsString();

		InputBuffer* buffer = _file_cache.Open(file_path.c_str());
		if (!buffer)
		{
			_error = string_util::Format("Failed to open shader file '%s'", file_path.c_str());
			return false;
		}

		D3DInclude include(_file_cache, _base_path, file_path);

		ID3DBlob* bytecode = NULL;
		ID3DBlob* errors = NULL;
		HRESULT hr = D3DCompile(buffer->Ptr(), (SIZE_T)buffer->Length(), file_path.c_str(), macros.data(), &include, program_node["entry"].AsString(),
			program_node["target"].AsString(), hlsl_flags, 0, &bytecode, &errors);

		if (FAILED(hr))
		{
			if (errors)
			{
				stringstream ss;
				ss << "\n" << (char*)errors->GetBufferPointer();
				_error = ss.str();

				// Remove \n that dx added at the end
				size_t p = _error.rfind('\n');
				_error.erase(p, 1);

				// Do some formatting
				p = _error.find('\n');
				while (p != string::npos)
				{
					_error.insert(p + 1, '\t', 1);
					p = _error.find('\n', p + 1);
				}


				errors->Release();
			}
			else
			{
				_error = string_util::Format("Failed to compile shader (HRESULT: 0x%x).", hr);
			}
			return false;
		}

		program.byte_code.clear();
		program.byte_code.insert(
			program.byte_code.end(),
			(uint8_t*)bytecode->GetBufferPointer(),
			((uint8_t*)bytecode->GetBufferPointer()) + bytecode->GetBufferSize());

		bytecode->Release();
		bytecode = NULL;

		return true;
	}
	void D3D11Platform::ReflectProgram(ShaderData& shader_data, D3D11ShaderPassData& d3d_shader_pass_data,
		D3D11ShaderPassData::ShaderProgram& shader_program)
	{
		ID3D11ShaderReflection* reflection = 0;
		HRESULT hr = D3DReflect(shader_program.byte_code.data(), shader_program.byte_code.size(), IID_ID3D11ShaderReflection, (void**)&reflection);
		if (FAILED(hr))
		{
			logging::Warning("Failed to reflect shader program (HRESULT: 0x%x).", hr);
			return;
		}

		D3D11_SHADER_DESC desc;
		reflection->GetDesc(&desc);

		for (uint32_t i = 0; i < desc.BoundResources; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC resource_desc;
			reflection->GetResourceBindingDesc(i, &resource_desc);

			ShaderResourceReflection::Resource* resource_reflection = NULL;

			// Check if the resource already exists in the resource reflection, this happends when there's multiple shader passes sharing the same variables.
			for (auto& refl : shader_data.resource_reflection.resources)
			{
				if (refl.name == resource_desc.Name)
				{
					resource_reflection = &refl;
					break;
				}
			}

			if (resource_desc.Type == D3D_SIT_TEXTURE)
			{
				if (resource_desc.BindCount != 1)
					logging::Warning("Resource arrays not implemented");

				if (!resource_reflection)
				{
					shader_data.resource_reflection.resources.push_back(ShaderResourceReflection::Resource());
					resource_reflection = &shader_data.resource_reflection.resources.back();
					resource_reflection->index = (uint32_t)shader_data.resource_reflection.resources.size() - 1;
					resource_reflection->name = resource_desc.Name;
				}

				D3D11ShaderPassData::ShaderProgram::ResourceBindInfo bind_info;
				bind_info.bind_point = resource_desc.BindPoint;
				bind_info.bind_count = resource_desc.BindCount;
				bind_info.index = resource_reflection->index;
				bind_info.flags = 0;

				if (resource_desc.Dimension == D3D_SRV_DIMENSION_TEXTURE2D || resource_desc.Dimension == D3D_SRV_DIMENSION_TEXTURE1D
					|| resource_desc.Dimension == D3D_SRV_DIMENSION_TEXTURE3D || resource_desc.Dimension == D3D_SRV_DIMENSION_TEXTURECUBE
					|| resource_desc.Dimension == D3D_SRV_DIMENSION_TEXTURE2DARRAY)
				{
					// Find sampler state bound to this texture
					map<string, string>::iterator it = _texture_to_state.find(resource_desc.Name);
					if (it != _texture_to_state.end())
					{
						D3D11ShaderPassData::SamplerState sampler_state = _sampler_states[it->second];

						if (sampler_state.srgb)
						{
							bind_info.flags |= D3D11ShaderPassData::TEXTURE_SRGB;
						}
					}
				}
				else if (resource_desc.Dimension == D3D_SRV_DIMENSION_BUFFER)
				{
				}
				else
				{
					logging::Warning("Unsupported dimension for resource '%s'.", resource_desc.Name);
				}

				shader_program.resource_bind_info.push_back(bind_info);

			}
			else if (resource_desc.Type == D3D_SIT_STRUCTURED || resource_desc.Type == D3D_SIT_UAV_RWSTRUCTURED)
			{
				if (resource_desc.BindCount != 1)
					logging::Warning("Resource arrays not implemented");

				if (!resource_reflection)
				{
					shader_data.resource_reflection.resources.push_back(ShaderResourceReflection::Resource());
					resource_reflection = &shader_data.resource_reflection.resources.back();
					resource_reflection->index = (uint32_t)shader_data.resource_reflection.resources.size() - 1;
					resource_reflection->name = resource_desc.Name;
				}

				D3D11ShaderPassData::ShaderProgram::ResourceBindInfo bind_info;
				bind_info.bind_point = resource_desc.BindPoint;
				bind_info.bind_count = resource_desc.BindCount;
				bind_info.index = resource_reflection->index;
				bind_info.flags = 0;

				if (resource_desc.Type == D3D_SIT_UAV_RWSTRUCTURED)
				{
					bind_info.flags |= D3D11ShaderPassData::TEXTURE_UAV;
				}

				shader_program.resource_bind_info.push_back(bind_info);
			}
			else if (resource_desc.Type == D3D_SIT_UAV_RWTYPED)
			{
				if (resource_desc.BindCount != 1)
					logging::Warning("Resource arrays not implemented");

				if (!resource_reflection)
				{
					shader_data.resource_reflection.resources.push_back(ShaderResourceReflection::Resource());
					resource_reflection = &shader_data.resource_reflection.resources.back();
					resource_reflection->index = (uint32_t)shader_data.resource_reflection.resources.size() - 1;
					resource_reflection->name = resource_desc.Name;
				}

				D3D11ShaderPassData::ShaderProgram::ResourceBindInfo bind_info;
				bind_info.bind_point = resource_desc.BindPoint;
				bind_info.bind_count = resource_desc.BindCount;
				bind_info.index = resource_reflection->index;
				bind_info.flags = 0;

				if (resource_desc.Dimension == D3D_SRV_DIMENSION_TEXTURE2D || resource_desc.Dimension == D3D_SRV_DIMENSION_TEXTURE1D
					|| resource_desc.Dimension == D3D_SRV_DIMENSION_TEXTURE3D || resource_desc.Dimension == D3D_SRV_DIMENSION_TEXTURECUBE
					|| resource_desc.Dimension == D3D_SRV_DIMENSION_TEXTURE2DARRAY)
				{
					bind_info.flags |= D3D11ShaderPassData::TEXTURE_UAV;

					// Find sampler state bound to this texture
					map<string, string>::iterator it = _texture_to_state.find(resource_desc.Name);
					if (it != _texture_to_state.end())
					{
						D3D11ShaderPassData::SamplerState sampler_state = _sampler_states[it->second];

						if (sampler_state.srgb)
						{
							bind_info.flags |= D3D11ShaderPassData::TEXTURE_SRGB;
						}
					}
				}
				else
				{
					logging::Warning("Unsupported dimension for UAV resource '%s'.", resource_desc.Name);
					continue;
				}

				shader_program.resource_bind_info.push_back(bind_info);
			}
			else if (resource_desc.Type == D3D_SIT_CBUFFER)
			{
				D3D11ShaderPassData::ShaderProgram::ConstantBufferBindInfo bind_info;
				bind_info.bind_point = resource_desc.BindPoint;
				bind_info.bind_count = resource_desc.BindCount;
				bind_info.index = Invalid<uint32_t>();

				// Check for existing reflections of this buffer
				for (uint32_t cr = 0; cr < shader_data.constant_buffer_reflections.size(); ++cr)
				{
					if (shader_data.constant_buffer_reflections[cr].name == resource_desc.Name)
					{
						bind_info.index = cr;
						break;
					}
				}
				if (IsInvalid(bind_info.index)) // No reflection found for this buffer, create a new one
				{
					shader_data.constant_buffer_reflections.push_back(ConstantBufferReflection());
					ConstantBufferReflection& constant_buffer_reflection = shader_data.constant_buffer_reflections.back();
					constant_buffer_reflection.name = resource_desc.Name;

					ID3D11ShaderReflectionConstantBuffer* d3d_constant_buffer_reflection = reflection->GetConstantBufferByName(resource_desc.Name);
					ReflectConstantBuffer(d3d_constant_buffer_reflection, constant_buffer_reflection);

					if (constant_buffer_reflection.type == RConstantBuffer::TYPE_LOCAL)
					{
						// Create constant buffer template
						ConstantBufferTemplate constant_buffer_template;
						constant_buffer_template.render_resource = RConstantBuffer(constant_buffer_reflection.size, RConstantBuffer::TYPE_LOCAL);
						constant_buffer_template.data = memory::Malloc(constant_buffer_reflection.size);

						FillConstantBuffer(constant_buffer_reflection, constant_buffer_template);
						shader_data.constant_buffer_templates.push_back(constant_buffer_template);
					}
					else
					{
						// We don't set any default data for global constant buffers
						ConstantBufferTemplate constant_buffer_template;
						constant_buffer_template.render_resource = RConstantBuffer(constant_buffer_reflection.size, RConstantBuffer::TYPE_GLOBAL);
						constant_buffer_template.data = NULL;
						shader_data.constant_buffer_templates.push_back(constant_buffer_template);
					}

					bind_info.index = (uint32_t)shader_data.constant_buffer_reflections.size() - 1;
				}

				shader_program.constant_buffer_bind_info.push_back(bind_info);

			}
			else if (resource_desc.Type == D3D_SIT_SAMPLER)
			{
				D3D11ShaderPassData::ShaderProgram::SamplerBindInfo bind_info;
				bind_info.bind_point = resource_desc.BindPoint;
				bind_info.bind_count = resource_desc.BindCount;
				bind_info.name = resource_desc.Name;

				shader_program.sampler_bind_info.push_back(bind_info);

				// Make sure state is available
				map<string, D3D11ShaderPassData::SamplerState>::iterator sampler_state_it = _sampler_states.find(resource_desc.Name);
				if (sampler_state_it == _sampler_states.end())
				{
					logging::Warning("Could not find sampler state '%s', setting to default.", resource_desc.Name);

					D3D11ShaderPassData::SamplerState state = _default_sampler_state;
					state.name = resource_desc.Name;
					d3d_shader_pass_data.sampler_states.push_back(state);
				}
				else
				{
					d3d_shader_pass_data.sampler_states.push_back(sampler_state_it->second);
				}

			}
			else
			{
				logging::Warning("Skipping reflection for resource of type %d: Not implemented", resource_desc.Type);
			}
		}

		reflection->Release();

	}
	void D3D11Platform::ReflectConstantBuffer(ID3D11ShaderReflectionConstantBuffer* d3d_reflection, ConstantBufferReflection& buffer_reflection)
	{
		D3D11_SHADER_BUFFER_DESC buffer_desc;
		d3d_reflection->GetDesc(&buffer_desc);

		buffer_reflection.size = buffer_desc.Size;
		buffer_reflection.name = buffer_desc.Name;
		buffer_reflection.type = RConstantBuffer::TYPE_LOCAL;

		// Check if the buffer is defined as a global buffer
		if (strncmp(buffer_desc.Name, "global_", 7) == 0)
		{
			buffer_reflection.type = RConstantBuffer::TYPE_GLOBAL;
		}

		for (uint32_t i = 0; i < buffer_desc.Variables; ++i)
		{
			ID3D11ShaderReflectionVariable* var = d3d_reflection->GetVariableByIndex(i);

			D3D11_SHADER_VARIABLE_DESC var_desc;
			var->GetDesc(&var_desc);

			ID3D11ShaderReflectionType* type = var->GetType();

			D3D11_SHADER_TYPE_DESC type_desc;
			type->GetDesc(&type_desc);

			ShaderVariable shader_var;
			shader_var.name = var_desc.Name;
			shader_var.offset = var_desc.StartOffset;
			shader_var.var_class = ShaderVariable::UNKNOWN;
			shader_var.elements = (type_desc.Elements == 0) ? 1 : type_desc.Elements;

			if (type_desc.Class == D3D_SVC_SCALAR)
			{
				if (type_desc.Elements == 0)
				{
					shader_var.var_class = ShaderVariable::SCALAR;
				}
				else
				{
					shader_var.var_class = ShaderVariable::SCALAR_ARRAY;
				}

			}
			else if (type_desc.Class == D3D_SVC_VECTOR)
			{
				if (type_desc.Columns == 2)
				{
					if (type_desc.Elements == 0)
						shader_var.var_class = ShaderVariable::VECTOR2;
					else
						shader_var.var_class = ShaderVariable::VECTOR2_ARRAY;

				}
				else if (type_desc.Columns == 3)
				{
					if (type_desc.Elements == 0)
						shader_var.var_class = ShaderVariable::VECTOR3;
					else
						shader_var.var_class = ShaderVariable::VECTOR3_ARRAY;
				}
				else if (type_desc.Columns == 4)
				{
					if (type_desc.Elements == 0)
						shader_var.var_class = ShaderVariable::VECTOR4;
					else
						shader_var.var_class = ShaderVariable::VECTOR4_ARRAY;
				}
				else
				{
					logging::Warning("Constant buffer '%s': Only vectors with 2, 3 or 4 columns supported.", buffer_desc.Name);
				}

			}
			else if (type_desc.Class == D3D_SVC_MATRIX_COLUMNS || type_desc.Class == D3D_SVC_MATRIX_ROWS)
			{
				if (type_desc.Columns == 4 || type_desc.Rows == 4)
				{
					if (type_desc.Elements == 0)
						shader_var.var_class = ShaderVariable::MATRIX4X4;
					else
						shader_var.var_class = ShaderVariable::MATRIX4X4_ARRAY;
				}
				else
				{
					logging::Warning("Constant buffer '%s': Only 4x4 matrices supported.", buffer_desc.Name);
				}
			}
			else
			{
				logging::Warning("Constant buffer '%s': Unknown variable type '%u'.", buffer_desc.Name, type_desc.Type);
			}

			buffer_reflection.variables.push_back(shader_var);
		}
	}
	//-------------------------------------------------------------------------------
	void D3D11Platform::FillConstantBuffer(const ConstantBufferReflection& buffer_reflection, ConstantBufferTemplate& constant_buffer_template)
	{
		memset(constant_buffer_template.data, 0, constant_buffer_template.render_resource.GetSize());

		StaticMemoryStream buffer_stream(constant_buffer_template.data, constant_buffer_template.render_resource.GetSize());
		for (auto& var : buffer_reflection.variables)
		{
			map<StringId32, VariableValue>::const_iterator value_it = _variables.find(var.name);
			if (value_it != _variables.end())
			{
				buffer_stream.Seek(var.offset);
				switch (value_it->second.var_class)
				{
				case ShaderVariable::SCALAR:
					buffer_stream.Write(&value_it->second.scalar, sizeof(float));
					break;
				case ShaderVariable::VECTOR2:
					buffer_stream.Write(&value_it->second.v2, sizeof(Vec2f));
					break;
				case ShaderVariable::VECTOR3:
					buffer_stream.Write(&value_it->second.v3, sizeof(Vec3f));
					break;
				case ShaderVariable::VECTOR4:
					buffer_stream.Write(&value_it->second.v4, sizeof(Vec4f));
					break;
				case ShaderVariable::MATRIX4X4:
					buffer_stream.Write(&value_it->second.m44, sizeof(Mat4x4f));
					break;
				}
			}
		}
	}
	//-------------------------------------------------------------------------------
	void D3D11Platform::ParseRenderState(const ConfigValue& object, D3D11ShaderPassData::RenderState& render_state)
	{
		// Set default
		render_state = _default_render_state;

		if (object.IsObject())
		{
			for (ConfigValue::ConstIterator it = object.Begin(); it != object.End(); ++it)
			{
				if (!it->second.IsString())
				{
					logging::Warning("Invalid render state variable type, expecting string.");
					continue;
				}

				// Blend state
				if (it->first == "blend_enable")
				{
					render_state.blend_desc.RenderTarget[0].BlendEnable = render_state::ParseBool(it->second.AsString());
				}
				else if (it->first == "src_blend")
				{
					render_state.blend_desc.RenderTarget[0].SrcBlend = render_state::ParseBlend(it->second.AsString());
				}
				else if (it->first == "dest_blend")
				{
					render_state.blend_desc.RenderTarget[0].DestBlend = render_state::ParseBlend(it->second.AsString());
				}
				else if (it->first == "blend_op")
				{
					render_state.blend_desc.RenderTarget[0].BlendOp = render_state::ParseBlendOp(it->second.AsString());
				}
				else if (it->first == "src_blend_alpha")
				{
					render_state.blend_desc.RenderTarget[0].SrcBlendAlpha = render_state::ParseBlend(it->second.AsString());
				}
				else if (it->first == "dest_blend_alpha")
				{
					render_state.blend_desc.RenderTarget[0].DestBlendAlpha = render_state::ParseBlend(it->second.AsString());
				}
				else if (it->first == "blend_op_alpha")
				{
					render_state.blend_desc.RenderTarget[0].BlendOpAlpha = render_state::ParseBlendOp(it->second.AsString());
				}

				// Depth stencil state
				else if (it->first == "depth_enable")
				{
					render_state.depth_stencil_desc.DepthEnable = render_state::ParseBool(it->second.AsString());
				}
				else if (it->first == "depth_write_enable")
				{
					render_state.depth_stencil_desc.DepthWriteMask = (render_state::ParseBool(it->second.AsString())
						? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO);
				}
				else if (it->first == "depth_func")
				{
					render_state.depth_stencil_desc.DepthFunc = render_state::ParseCompareFunc(it->second.AsString());
				}
				else if (it->first == "stencil_enable")
				{
					render_state.depth_stencil_desc.StencilEnable = render_state::ParseBool(it->second.AsString());
				}
				else if (it->first == "stencil_read_mask")
				{
					render_state.depth_stencil_desc.StencilReadMask = render_state::ParseMask8(it->second.AsString());
				}
				else if (it->first == "stencil_write_mask")
				{
					render_state.depth_stencil_desc.StencilWriteMask = render_state::ParseMask8(it->second.AsString());
				}
				else if (it->first == "stencil_fail_op")
				{
					D3D11_STENCIL_OP op = render_state::ParseStencilOp(it->second.AsString());
					render_state.depth_stencil_desc.FrontFace.StencilFailOp = op;
					render_state.depth_stencil_desc.BackFace.StencilFailOp = op;
				}
				else if (it->first == "stencil_depth_fail_op")
				{
					D3D11_STENCIL_OP op = render_state::ParseStencilOp(it->second.AsString());
					render_state.depth_stencil_desc.FrontFace.StencilDepthFailOp = op;
					render_state.depth_stencil_desc.BackFace.StencilDepthFailOp = op;
				}
				else if (it->first == "stencil_pass_op")
				{
					D3D11_STENCIL_OP op = render_state::ParseStencilOp(it->second.AsString());
					render_state.depth_stencil_desc.FrontFace.StencilPassOp = op;
					render_state.depth_stencil_desc.BackFace.StencilPassOp = op;
				}
				else if (it->first == "stencil_func")
				{
					D3D11_COMPARISON_FUNC func = render_state::ParseCompareFunc(it->second.AsString());
					render_state.depth_stencil_desc.FrontFace.StencilFunc = func;
					render_state.depth_stencil_desc.BackFace.StencilFunc = func;
				}

				// Rasterizer state
				else if (it->first == "fill_mode")
				{
					render_state.rasterizer_desc.FillMode = render_state::ParseFillMode(it->second.AsString());
				}
				else if (it->first == "cull_mode")
				{
					render_state.rasterizer_desc.CullMode = render_state::ParseCullMode(it->second.AsString());
				}
				else if (it->first == "scissor_enable")
				{
					render_state.rasterizer_desc.ScissorEnable = render_state::ParseBool(it->second.AsString());
				}
				else
				{
					logging::Warning("'%s' is not a valid render state variable.", it->first.c_str());
				}
			}
		}
	}
	void D3D11Platform::ParseSamplerState(const ConfigValue& object, D3D11ShaderPassData::SamplerState& sampler_state)
	{
		sampler_state = _default_sampler_state;

		if (object.IsObject())
		{
			for (ConfigValue::ConstIterator it = object.Begin(); it != object.End(); ++it)
			{
				if (!it->second.IsString())
				{
					logging::Warning("Invalid render state variable type, expecting string.");
					continue;
				}

				// Blend state
				if (it->first == "filter")
				{
					sampler_state.desc.Filter = render_state::ParseFilter(it->second.AsString());
				}
				else if (it->first == "address_u")
				{
					sampler_state.desc.AddressU = render_state::ParseTAM(it->second.AsString());
				}
				else if (it->first == "address_v")
				{
					sampler_state.desc.AddressV = render_state::ParseTAM(it->second.AsString());
				}
				else if (it->first == "address_w")
				{
					sampler_state.desc.AddressW = render_state::ParseTAM(it->second.AsString());
				}
				else if (it->first == "comparison_func")
				{
					sampler_state.desc.ComparisonFunc = render_state::ParseCompareFunc(it->second.AsString());
				}
				else if (it->first == "srgb")
				{
					sampler_state.srgb = render_state::ParseBool(it->second.AsString());
				}
				else
				{
					logging::Warning("'%s' is not a valid sampler state variable.", it->first.c_str());
				}
			}
		}
	}
	void D3D11Platform::ParseInstanceData(const ConfigValue& instance_data_node, 
										  InstanceDataReflection& instance_data_reflection,
										  D3D11ShaderPassData::InstanceDataBindInfo& instance_data_bind_info)
	{
		if (instance_data_node.IsArray())
		{
			for (uint32_t i = 0; i < instance_data_node.Size(); ++i)
			{
				const ConfigValue& var = instance_data_node[i];
				if (!var.IsObject() || !var["name"].IsString() || !var["type"].IsString())
					continue;

				StringId32 name = var["name"].AsString();
				ShaderVariable::Class var_type = StringToType(var["type"].AsString());
				uint32_t type_size = shader_variable::GetSize(var_type);
				if (var_type == ShaderVariable::UNKNOWN)
				{
					logging::Warning("Skipping instance data variable '%s' : Invalid variable type!", var["name"].AsString());
					continue;
				}

				// Check if the variable is already set in the instance data reflection
				vector<ShaderVariable>::iterator idv_it, idv_end;
				idv_it = instance_data_reflection.variables.begin(); idv_end = instance_data_reflection.variables.end();
				for (; idv_it != idv_end; ++idv_it)
				{
					if (idv_it->name == name)
						break;
				}

				D3D11ShaderPassData::InstanceDataBindInfo::Variable d3d_variable;
				if (idv_it != idv_end)
				{
					// Found match!

					// Make sure size matches
					if (var_type != idv_it->var_class)
					{
						logging::Warning("Skipping instance data variable '%s' : Variable with same name but diffrent type found!", var["name"].AsString());
						continue;
					}

					d3d_variable.src_offset = idv_it->offset;
					d3d_variable.dest_offset = instance_data_bind_info.size;
					d3d_variable.size = type_size;
				}
				else
				{
					// Not found, therefore we need to insert reflection into both InstanceDataReflection and d3d reflection.

					d3d_variable.src_offset = instance_data_reflection.size;
					d3d_variable.dest_offset = instance_data_bind_info.size;
					d3d_variable.size = type_size;

					ShaderVariable refl_var;
					refl_var.var_class = var_type;
					refl_var.offset = instance_data_reflection.size;
					refl_var.name = name;
					refl_var.elements = 1;

					instance_data_reflection.variables.push_back(refl_var);
					instance_data_reflection.size += d3d_variable.size;
				}

				instance_data_bind_info.variables.push_back(d3d_variable);
				instance_data_bind_info.size += d3d_variable.size;
			}
		}
	}
	//-------------------------------------------------------------------------------


} // namespace sb
