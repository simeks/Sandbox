// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Shader.h"

#include <Engine/Rendering/RenderResourceAllocator.h>
#include <Foundation/Filesystem/File.h>

namespace sb
{

	//-------------------------------------------------------------------------------

	ConstantBufferReflection::ConstantBufferReflection()
		: size(0),
		type(RConstantBuffer::TYPE_LOCAL)
	{
	}
	ConstantBufferReflection::~ConstantBufferReflection()
	{
	}
	void ConstantBufferReflection::Serialize(Stream& stream)
	{
		uint32_t count = (uint32_t)variables.size();
		stream.Write(&count, 4);
		stream.Write(variables.data(), sizeof(ShaderVariable)*count);
		stream.Write(&size, 4);
		stream.Write(&type, sizeof(RConstantBuffer::Type));
		stream.Write(&name, 4);
	}
	void ConstantBufferReflection::Deserialize(Stream& stream)
	{
		uint32_t count;
		stream.Read(&count, 4);
		variables.resize(count);
		stream.Read(variables.data(), sizeof(ShaderVariable)*count);
		stream.Read(&size, 4);
		stream.Read(&type, sizeof(RConstantBuffer::Type));
		stream.Read(&name, 4);
	}

	//-------------------------------------------------------------------------------

	InstanceDataReflection::InstanceDataReflection()
		: size(0)
	{
	}
	InstanceDataReflection::~InstanceDataReflection()
	{
	}
	void InstanceDataReflection::Serialize(Stream& stream)
	{
		uint32_t count = (uint32_t)variables.size();
		stream.Write(&count, 4);
		if (count)
		{
			stream.Write(variables.data(), sizeof(ShaderVariable)*count);
			stream.Write(&size, 4);
		}
	}
	void InstanceDataReflection::Deserialize(Stream& stream)
	{
		uint32_t count;
		stream.Read(&count, 4);
		if (count)
		{
			variables.resize(count);
			stream.Read(variables.data(), sizeof(ShaderVariable)*count);
			stream.Read(&size, 4);
		}
	}

	//-------------------------------------------------------------------------------
	ShaderData::ShaderData()
		: name("")
	{
	}
	ShaderData::~ShaderData()
	{
	}

	//-------------------------------------------------------------------------------

	Shader::Shader(ShaderData* data, RShader* render_resource)
		: _data(data),
		_render_resource(render_resource),
		_resource_binder(data)
	{
	}
	Shader::~Shader()
	{
		AssertMsg(!_resource_blocks.size(), "Unreleased shader contexts");
	}

	ShaderContext* Shader::CreateContext()
	{
		ShaderContext* context = new ShaderContext();
		context->shader = _render_resource;
		context->resources = AllocateResourceBlock();

		return context;
	}
	void Shader::ReleaseContext(ShaderContext* context)
	{
		ReleaseResourceBlock(context->resources);
		delete context;
	}

	const ShaderResourceBinder& Shader::GetShaderResourceBinder() const
	{
		return _resource_binder;
	}
	ShaderResourceBinder& Shader::GetShaderResourceBinder()
	{
		return _resource_binder;
	}
	const ShaderData* Shader::GetData() const
	{
		return _data;
	}

	ShaderResources* Shader::AllocateResourceBlock()
	{
		ShaderResources* resources = new ShaderResources();
		resources->num_resources = (uint32_t)_data->resource_reflection.resources.size();
		resources->resources = nullptr;

		resources->num_constant_buffers = (uint32_t)_data->constant_buffer_reflections.size();
		resources->constant_buffers = nullptr;
		resources->constant_buffer_data = nullptr;

		if (resources->num_resources)
		{
			resources->resources = new RenderResource[resources->num_resources];
		}

		for (uint32_t i = 0; i < resources->num_resources; ++i)
		{
			resources->resources[i].SetHandle(Invalid<uint32_t>());
		}

		uint32_t total_size = 0;
		for (uint32_t i = 0; i < resources->num_constant_buffers; ++i)
		{
			total_size += ((_data->constant_buffer_reflections[i].type != RConstantBuffer::TYPE_GLOBAL) ? _data->constant_buffer_reflections[i].size : 0);
		}

		if (total_size)
		{
			resources->constant_buffer_data = memory::Malloc(total_size);
		}

		if (_data->instance_data_reflection.size)
		{
			resources->instance_data_size = _data->instance_data_reflection.size;
			resources->instance_data = memory::Malloc(_data->instance_data_reflection.size);
		}

		if (resources->num_constant_buffers)
		{
			resources->constant_buffers = new RConstantBuffer[resources->num_constant_buffers];
		}

		uint32_t cb_offset = 0;
		for (uint32_t i = 0; i < resources->num_constant_buffers; ++i)
		{
			uint32_t size = ((_data->constant_buffer_reflections[i].type != RConstantBuffer::TYPE_GLOBAL) ? _data->constant_buffer_reflections[i].size : 0);
			void* cb_data = nullptr;
			if (size != 0)
			{
				cb_data = memory::PointerAdd(resources->constant_buffer_data, cb_offset);
				memcpy(cb_data, _data->constant_buffer_templates[i].data, size);
			}
			cb_offset += size;

			// We allocate new data for the buffer but we keep the handle as all buffers inheriting a template shares the same hardware buffer
			resources->constant_buffers[i] = _data->constant_buffer_templates[i].render_resource;
		}

		_resource_blocks.push_back(resources);

		return resources;
	}
	void Shader::ReleaseResourceBlock(ShaderResources* resources)
	{
		vector<ShaderResources*>::iterator it = std::find(_resource_blocks.begin(), _resource_blocks.end(), resources);
		Assert(it != _resource_blocks.end()); // Could not find the allocated resources in this shader.

		_resource_blocks.erase(it);

		if (resources->constant_buffer_data)
		{
			memory::Free(resources->constant_buffer_data);
		}

		if (resources->instance_data)
		{
			memory::Free(resources->instance_data);
		}

		delete[] resources->constant_buffers;
		delete[] resources->resources;

		delete resources;
	}

} // namespace sb
