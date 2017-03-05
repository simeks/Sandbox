// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11SHADERDATA_H__
#define __D3D11SHADERDATA_H__

#include <Foundation/IO/MemoryStream.h>


namespace sb
{

	struct D3D11ShaderPassData
	{
		enum ResourceFlags
		{
			TEXTURE_SRGB = 0x1, ///< Enable SRGB for texture
			TEXTURE_UAV = 0x2 ///< Use unordered access
		};
		struct ShaderProgram
		{
			struct ResourceBindInfo
			{
				uint32_t bind_point;
				uint32_t bind_count;
				uint32_t index;
				uint32_t flags;
			};
			struct ConstantBufferBindInfo
			{
				uint32_t bind_point;
				uint32_t bind_count;
				uint32_t index;
			};
			struct SamplerBindInfo
			{
				StringId32 name;
				uint32_t bind_point;
				uint32_t bind_count;
			};

			vector<uint8_t> byte_code;
			vector<ResourceBindInfo> resource_bind_info;
			vector<ConstantBufferBindInfo> constant_buffer_bind_info;
			vector<SamplerBindInfo> sampler_bind_info;

			ShaderProgram();

			void Serialize(Stream& stream);
			void Deserialize(Stream& stream);
		};

		struct RenderState
		{
			D3D11_BLEND_DESC blend_desc;
			float blend_factor[4];
			uint32_t sample_mask;

			D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
			uint32_t stencil_ref;

			D3D11_RASTERIZER_DESC rasterizer_desc;
		};

		struct SamplerState
		{
			StringId32 name;
			D3D11_SAMPLER_DESC desc;
			bool srgb;
		};

		struct InstanceDataBindInfo
		{
			struct Variable
			{
				uint32_t src_offset; ///< Offset in the source buffer
				uint32_t dest_offset;  ///< Offset in the destination buffer
				uint32_t size; ///< Size in bytes
			};

			InstanceDataBindInfo();
			/// Serializes the instance info data to a byte stream
			void Serialize(Stream& stream);
			/// Deserializes the instance info data from a stream of bytes
			void Deserialize(Stream& stream);

			vector<Variable> variables;
			uint32_t size;

			uint32_t instance_data_slot; // Resource slot for the instance data buffer.
		};


		D3D11ShaderPassData();
		~D3D11ShaderPassData();

		/// Serializes the shader data to a byte stream
		void Serialize(Stream& stream);
		/// Deserializes the shader data from a stream of bytes
		void Deserialize(Stream& stream);


		RenderState render_state;
		vector<SamplerState> sampler_states;

		ShaderProgram vertex_shader;
		ShaderProgram hull_shader;
		ShaderProgram domain_shader;
		ShaderProgram geometry_shader;
		ShaderProgram pixel_shader;
		ShaderProgram compute_shader;

		InstanceDataBindInfo instance_data;
	};

	/// Platform-specific data describing a shader
	struct D3D11ShaderData
	{
		D3D11ShaderData();
		~D3D11ShaderData();

		vector<D3D11ShaderPassData> passes;


		/// Serializes the shader data to a byte stream
		void Serialize(Stream& stream);
		/// Deserializes the shader data from a stream of bytes
		void Deserialize(Stream& stream);

	};


} // namespace sb



#endif // __D3D11SHADERDATA_H__
