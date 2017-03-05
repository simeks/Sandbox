// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "D3D11ShaderData.h"


namespace sb
{

	//-------------------------------------------------------------------------------

	D3D11ShaderPassData::ShaderProgram::ShaderProgram()
	{
	}

	void D3D11ShaderPassData::ShaderProgram::Serialize(Stream& stream)
	{
		uint32_t size = (uint32_t)byte_code.size();
		stream.Write(&size, 4);
		if (size != 0)
		{
			stream.Write(byte_code.data(), size);

			uint32_t count = (uint32_t)resource_bind_info.size();
			stream.Write(&count, 4);
			stream.Write(resource_bind_info.data(), count*sizeof(ResourceBindInfo));

			count = (uint32_t)constant_buffer_bind_info.size();
			stream.Write(&count, 4);
			stream.Write(constant_buffer_bind_info.data(), count*sizeof(ConstantBufferBindInfo));

			count = (uint32_t)sampler_bind_info.size();
			stream.Write(&count, 4);
			stream.Write(sampler_bind_info.data(), count*sizeof(SamplerBindInfo));
		}
	}
	void D3D11ShaderPassData::ShaderProgram::Deserialize(Stream& stream)
	{
		uint32_t size = 0;
		// Shader byte-code
		stream.Read(&size, 4);
		if (size > 0)
		{
			byte_code.resize(size);
			stream.Read(byte_code.data(), size);

			// Resource bind info
			uint32_t count = 0;
			stream.Read(&count, 4);
			if (count)
			{
				resource_bind_info.resize(count);
				stream.Read(resource_bind_info.data(), sizeof(ResourceBindInfo)*count);
			}

			count = 0;
			stream.Read(&count, 4);
			if (count)
			{
				constant_buffer_bind_info.resize(count);
				stream.Read(constant_buffer_bind_info.data(), sizeof(ConstantBufferBindInfo)*count);
			}

			count = 0;
			stream.Read(&count, 4);
			if (count)
			{
				sampler_bind_info.resize(count);
				stream.Read(sampler_bind_info.data(), sizeof(SamplerBindInfo)*count);
			}
		}
	}

	//-------------------------------------------------------------------------------
	D3D11ShaderPassData::InstanceDataBindInfo::InstanceDataBindInfo()
		: size(0), instance_data_slot(Invalid<uint32_t>())
	{
	}
	void D3D11ShaderPassData::InstanceDataBindInfo::Serialize(Stream& stream)
	{
		uint32_t count = (uint32_t)variables.size();
		stream.Write(&count, 4);
		if (count)
		{
			stream.Write(variables.data(), sizeof(Variable)*count);
		}

		stream.Write(&size, 4);
		stream.Write(&instance_data_slot, 4);
	}
	void D3D11ShaderPassData::InstanceDataBindInfo::Deserialize(Stream& stream)
	{
		uint32_t count = 0;
		stream.Read(&count, 4);
		if (count)
		{
			variables.resize(count);
			stream.Read(variables.data(), sizeof(Variable)*count);
		}

		stream.Read(&size, 4);
		stream.Read(&instance_data_slot, 4);
	}
	//-------------------------------------------------------------------------------



	D3D11ShaderPassData::D3D11ShaderPassData()
	{
	}
	D3D11ShaderPassData::~D3D11ShaderPassData()
	{
	}

	void D3D11ShaderPassData::Serialize(Stream& stream)
	{
		stream.Write(&render_state, sizeof(RenderState));

		uint32_t count = (uint32_t)sampler_states.size();
		stream.Write(&count, 4);
		for (uint32_t i = 0; i < count; ++i)
		{
			stream.Write(&sampler_states[i].name, 4);
			stream.Write(&sampler_states[i].desc, sizeof(D3D11_SAMPLER_DESC));
			stream.Write(&sampler_states[i].srgb, 1);
		}

		vertex_shader.Serialize(stream);
		hull_shader.Serialize(stream);
		domain_shader.Serialize(stream);
		geometry_shader.Serialize(stream);
		pixel_shader.Serialize(stream);
		compute_shader.Serialize(stream);
		instance_data.Serialize(stream);
	}
	void D3D11ShaderPassData::Deserialize(Stream& stream)
	{
		stream.Read(&render_state, sizeof(RenderState));

		uint32_t count = (uint32_t)sampler_states.size();
		stream.Read(&count, 4);
		if (count)
		{
			sampler_states.resize(count);
			for (uint32_t i = 0; i < count; ++i)
			{
				stream.Read(&sampler_states[i].name, 4);
				stream.Read(&sampler_states[i].desc, sizeof(D3D11_SAMPLER_DESC));
				stream.Read(&sampler_states[i].srgb, 1);
			}
		}

		vertex_shader.Deserialize(stream);
		hull_shader.Deserialize(stream);
		domain_shader.Deserialize(stream);
		geometry_shader.Deserialize(stream);
		pixel_shader.Deserialize(stream);
		compute_shader.Deserialize(stream);
		instance_data.Deserialize(stream);
	}



	//-------------------------------------------------------------------------------
	D3D11ShaderData::D3D11ShaderData()
	{

	}
	D3D11ShaderData::~D3D11ShaderData()
	{
	}

	void D3D11ShaderData::Serialize(Stream& stream)
	{
		uint32_t pass_count = (uint32_t)passes.size();
		stream.Write(&pass_count, 4);

		vector<D3D11ShaderPassData>::iterator it, end;
		it = passes.begin(); end = passes.end();
		for (; it != end; ++it)
		{
			it->Serialize(stream);
		}
	}
	void D3D11ShaderData::Deserialize(Stream& stream)
	{
		uint32_t pass_count = 0;
		stream.Read(&pass_count, 4);
		passes.reserve(pass_count);

		for (uint32_t i = 0; i < pass_count; ++i)
		{
			passes.push_back(D3D11ShaderPassData());
			passes.back().Deserialize(stream);
		}
	}


	//-------------------------------------------------------------------------------

} // namespace sb

