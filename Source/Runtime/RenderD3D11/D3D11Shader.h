// Copyright 2008-2014 Simon Ekström

#ifndef __SANDBOX_D3D11SHADER_H__
#define __SANDBOX_D3D11SHADER_H__

#include "D3D11ShaderProgram.h"
#include "D3D11RenderStates.h"

struct ID3D11VertexShader;
struct ID3D11HullShader;
struct ID3D11DomainShader;
struct ID3D11GeometryShader;
struct ID3D11PixelShader;
struct ID3D11ComputeShader;

namespace sb
{

	class Shader;
	class D3D11ResourceManager;

	struct D3D11ShaderData;
	struct D3D11ShaderPassData;

	class D3D11ShaderPass
	{
	public:
		D3D11ShaderPass();
		~D3D11ShaderPass();

		/// Loads the shader from the specified Shader-object
		void Load(D3D11ResourceManager* resource_manager, D3D11ShaderPassData* pass_data);

		D3D11ShaderProgram<ID3D11VertexShader>* GetVertexShader();
		D3D11ShaderProgram<ID3D11HullShader>* GetHullShader();
		D3D11ShaderProgram<ID3D11DomainShader>* GetDomainShader();
		D3D11ShaderProgram<ID3D11GeometryShader>* GetGeometryShader();
		D3D11ShaderProgram<ID3D11PixelShader>* GetPixelShader();
		D3D11ShaderProgram<ID3D11ComputeShader>* GetComputeShader();

		D3D11BlendState* GetBlendState() const;
		D3D11DepthStencilState* GetDepthStencilState() const;
		D3D11RasterizerState* GetRasterizerState() const;
		D3D11SamplerState* GetSamplerState(StringId32 name) const;

		D3D11ShaderPassData* GetData();
		const D3D11ShaderPassData* GetData() const;

	private:
		D3D11ResourceManager* _resource_manager;

		D3D11BlendState* _blend_state;
		D3D11DepthStencilState* _depth_stencil_state;
		D3D11RasterizerState* _rasterizer_state;
		map<StringId32, D3D11SamplerState*> _sampler_states;

		D3D11ShaderProgram<ID3D11VertexShader>* _vertex_shader;
		D3D11ShaderProgram<ID3D11HullShader>* _hull_shader;
		D3D11ShaderProgram<ID3D11DomainShader>* _domain_shader;
		D3D11ShaderProgram<ID3D11GeometryShader>* _geometry_shader;
		D3D11ShaderProgram<ID3D11PixelShader>* _pixel_shader;
		D3D11ShaderProgram<ID3D11ComputeShader>* _compute_shader;

		D3D11ShaderPassData* _data;

	};

	class D3D11Shader
	{
	public:
		D3D11Shader();
		~D3D11Shader();

		/// Loads the shader from the specified Shader-object
		void Load(D3D11ResourceManager* resource_manager, void* platform_data, uint32_t platform_data_size);

		D3D11ShaderPass* GetShaderPass(uint32_t index);

	private:

		vector<D3D11ShaderPass*> _passes;
		D3D11ShaderData* _data;
	};


} // namespace sb



#endif // __SANDBOX_D3D11SHADER_H__

