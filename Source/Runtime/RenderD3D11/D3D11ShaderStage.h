// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11SHADERSTAGE_H__
#define __D3D11SHADERSTAGE_H__

#include "D3D11ShaderData.h"
#include "D3D11Buffer.h"
#include <Engine/Rendering/RenderContext.h>



namespace sb
{

	class RenderResource;
	class RConstantBuffer;
	class D3D11Shader;
	class D3D11ShaderPass;
	class D3D11DeviceContext;
	class D3D11ResourceManager;
	class D3D11ShaderStage
	{
	public:
		D3D11ShaderStage(D3D11DeviceContext* context);
		~D3D11ShaderStage();

		/// @param sort_key Sorting key from the drawcall, used to determined shader pass index
		void BindShader(D3D11Shader* shader, uint64_t sort_key);
		void BindResources(RenderResource* resources, uint32_t resource_count);
		void BindConstantBuffers(RConstantBuffer* buffers, uint32_t buffer_count, void* constant_data);

		/// Clears the current state.
		void Clear();

		/// Returns the bound shader pass
		D3D11ShaderPass* GetShaderPass();

	private:

		/// Binds any resources that needs to be bound to the given array of resource views
		/// @return True if the resource view array has changed, false if not
		bool BindProgramResources(RenderResource* resources, uint32_t resource_count, const vector<D3D11ShaderPassData::ShaderProgram::ResourceBindInfo>& bind_info,
			vector<ID3D11ShaderResourceView*>& resource_views, uint32_t& start, uint32_t& count);

		bool BindProgramSamplers(const vector<D3D11ShaderPassData::ShaderProgram::SamplerBindInfo>& bind_info, vector<ID3D11SamplerState*>& sampler_states,
			uint32_t& start, uint32_t& count);

		void BindProgramConstantBuffers(const vector<D3D11ShaderPassData::ShaderProgram::ConstantBufferBindInfo>& bind_info, const vector<D3D11ConstantBuffer*>& d3dbuffers, vector<ID3D11Buffer*>& outbuffers);


		D3D11DeviceContext* _context;
		D3D11ResourceManager* _resource_manager;

		D3D11Shader* _shader;
		D3D11ShaderPass* _shader_pass;

		ID3D11VertexShader* _vertex_shader;
		ID3D11HullShader* _hull_shader;
		ID3D11DomainShader* _domain_shader;
		ID3D11GeometryShader* _geometry_shader;
		ID3D11PixelShader* _pixel_shader;
		ID3D11ComputeShader* _compute_shader;


		vector<ID3D11ShaderResourceView*> _vs_resources;
		vector<ID3D11SamplerState*> _vs_sampler_states;

		vector<ID3D11ShaderResourceView*> _hs_resources;
		vector<ID3D11SamplerState*> _hs_sampler_states;

		vector<ID3D11ShaderResourceView*> _ds_resources;
		vector<ID3D11SamplerState*> _ds_sampler_states;

		vector<ID3D11ShaderResourceView*> _gs_resources;
		vector<ID3D11SamplerState*> _gs_sampler_states;

		vector<ID3D11ShaderResourceView*> _ps_resources;
		vector<ID3D11SamplerState*> _ps_sampler_states;

		vector<ID3D11ShaderResourceView*> _cs_resources;
		vector<ID3D11SamplerState*> _cs_sampler_states;


	};

} // namespace sb


#endif // __D3D11SHADERSTAGE_H__

