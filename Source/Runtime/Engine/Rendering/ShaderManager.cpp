// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ShaderManager.h"
#include "Shader.h"
#include "ShaderLibrary.h"

#include <Engine/Rendering/RenderDevice.h>
#include <Engine/Rendering/RenderContext.h>
#include <Engine/Rendering/RenderResourceAllocator.h>

namespace sb
{

	ShaderManager::ShaderManager(RenderDevice* device, ResourceManager* resource_manager)
		: _device(device)
	{
		shader_library_resource::RegisterResourceType(resource_manager, this);
	}
	ShaderManager::~ShaderManager()
	{
	}

	Shader* ShaderManager::GetShader(StringId32 name)
	{
		map<StringId32, Shader*>::iterator it = _shaders.find(name);
		if (it != _shaders.end())
		{
			return it->second;
		}
		return nullptr;
	}


	void ShaderManager::AddLibrary(ShaderLibrary* shader_library)
	{
		for (uint32_t p = 0; p < shader_library->permutation_count; ++p)
		{
			ShaderData* permutation = &shader_library->permutations[p];
			RShader* resource_handle = &shader_library->render_resources[p];

			Shader* shader = new Shader(permutation, resource_handle);

			map<StringId32, Shader*>::iterator it = _shaders.find(permutation->name);
			Assert(it == _shaders.end()); // Shader already loaded

			_shaders[permutation->name] = shader;
		}
	}
	void ShaderManager::RemoveLibrary(ShaderLibrary* shader_library)
	{
		for (uint32_t p = 0; p < shader_library->permutation_count; ++p)
		{
			ShaderData* permutation = &shader_library->permutations[p];

			map<StringId32, Shader*>::iterator it = _shaders.find(permutation->name);
			if (it != _shaders.end())
			{
				delete it->second;
				_shaders.erase(it);
			}
		}
	}


	void ShaderManager::UpdateConstantBuffers(RenderContext* context, const ShaderPerFrameData& per_frame_data)
	{
		for (auto& cb : _global_constant_buffers)
		{
			cb.second.binder.Bind(cb.second.data, per_frame_data);
			context->UpdateBuffer(0, cb.second.render_resource, cb.second.data);
		}
	}

	const RConstantBuffer& ShaderManager::CreateGlobalConstantBuffer(StringId32 name, const ConstantBufferReflection& reflection)
	{
		map<StringId32, GlobalConstantBuffer>::iterator it = _global_constant_buffers.find(name);
		if (it != _global_constant_buffers.end())
		{
			AssertMsg(it->second.reflection.size == reflection.size, "Cannot create two global constant buffers with same name but different layouts.");
			++it->second.refcount;
			return it->second.render_resource;
		}

		void* data = 0;
		if (reflection.size > 0)
		{
			data = memory::Malloc(reflection.size);
			memset(data, 0, reflection.size);
		}

		RConstantBuffer constant_buffer(reflection.size, RConstantBuffer::TYPE_GLOBAL);
		_device->GetResourceAllocator()->AllocateConstantBuffer(constant_buffer, data);

		it = _global_constant_buffers.insert(pair<StringId32, GlobalConstantBuffer>
			(name, GlobalConstantBuffer(reflection, constant_buffer))).first;
		it->second.refcount = 1;
		it->second.data = data;

		return it->second.render_resource;
	}

	void ShaderManager::ReleaseGlobalConstantBuffer(StringId32 name)
	{
		map<StringId32, GlobalConstantBuffer>::iterator it = _global_constant_buffers.find(name);
		if (it != _global_constant_buffers.end())
		{
			if (--it->second.refcount == 0)
			{
				if (it->second.data)
					memory::Free(it->second.data);

				_device->GetResourceAllocator()->ReleaseResource(it->second.render_resource);
				_global_constant_buffers.erase(it);
			}
		}
	}
	RenderResourceAllocator* ShaderManager::GetResourceAllocator()
	{
		return _device->GetResourceAllocator();
	}

} // namespace sb

