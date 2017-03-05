// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDER_D3D11PLATFORM_H__
#define __BUILDER_D3D11PLATFORM_H__

#include "CompilerSystem.h"
#include "ShaderCompiler.h"

#include <Engine/Rendering/ShaderVariable.h>

#include <d3d11.h>
#include <RenderD3D11/D3D11ShaderData.h>

struct ID3D11ShaderReflectionConstantBuffer;

namespace sb
{

	struct ConstantBufferReflection;
	struct InstanceDataReflection;

	class D3DInclude;
	struct ConstantBufferTemplate;

	class D3D11Platform
	{
	public:
		D3D11Platform(const string& base_path, const CompilerSystem::CompilerContext& context);
		~D3D11Platform();

		/// Precompiles the specified shader, mainly parsing any render states defined in the shader file.
		///	This should be performed before compiling any shaders with Compile.
		void Precompile(const ShaderDatabase::Shader& shader);

		/// @brief Compiles a shader and stores the result in shaderData
		/// @remark Remember to use Cleanup on the ShaderData when done with it.
		/// @sa Cleanup
		bool Compile(const ShaderDatabase::Shader& shader, const vector<string>& defines, ShaderData& shader_data);

		/// @brief Cleans up after the compilation, releasing any allocated data.
		void Cleanup(ShaderData& shader_data);

		ShaderCompiler::ShaderFileCache& GetCache();
		const string& GetError() const;

	private:
		bool CompilePass(const ConfigValue& pass_node, const vector<string>& options, ShaderData& shader_data,
			D3D11ShaderPassData& d3d_shader_pass_data);
		bool CompileProgram(const ConfigValue& program_node, const vector<D3D_SHADER_MACRO>& macros, UINT hlsl_flags, D3D11ShaderPassData::ShaderProgram& program);

		void ReflectProgram(ShaderData& shader_data, D3D11ShaderPassData& d3d_shader_pass_data, D3D11ShaderPassData::ShaderProgram& shader_program);
		void ReflectConstantBuffer(ID3D11ShaderReflectionConstantBuffer* d3d_reflection, ConstantBufferReflection& buffer_reflection);

		/// @brief Fills the specified constant buffer template with the default values.
		void FillConstantBuffer(const ConstantBufferReflection& buffer_reflection, ConstantBufferTemplate& constant_buffer_template);

		void ParseRenderState(const ConfigValue& object, D3D11ShaderPassData::RenderState& render_state);
		void ParseSamplerState(const ConfigValue& object, D3D11ShaderPassData::SamplerState& sampler_state);

		void ParseInstanceData(const ConfigValue& instance_data_node, InstanceDataReflection& instance_data_reflection,
			D3D11ShaderPassData::InstanceDataBindInfo& instance_data_bind_info);

	private:
		const CompilerSystem::CompilerContext& _context;
		ShaderCompiler::ShaderFileCache _file_cache;

		string _error;
		string _base_path;

		struct VariableValue
		{
			ShaderVariable::Class var_class;

			float scalar;
			Vec2f v2;
			Vec3f v3;
			Vec4f v4;
			Mat4x4f m44;
		};

		D3D11ShaderPassData::RenderState _default_render_state;
		D3D11ShaderPassData::SamplerState _default_sampler_state;

		map<string, D3D11ShaderPassData::RenderState> _render_states;
		map<string, D3D11ShaderPassData::SamplerState> _sampler_states;
		map<string, string> _texture_to_state; // Maps a texture resource to a sampler state

		map<StringId32, VariableValue> _variables; // Stores default values for the shader variables.
	};

} // namespace sb

#endif // __BUILDER_D3D11PLATFORM_H__
