// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ShaderLibrary.h"
#include "ShaderManager.h"

#include <Engine/Rendering/RenderResourceAllocator.h>
#include <Foundation/Filesystem/File.h>

namespace sb
{

	void shader_library_resource::Compile(const ShaderLibrary& shader_library, Stream& stream)
	{
		uint32_t version = SHADER_LIBRARY_RESOURCE_VERSION;
		stream.Write(&version, 4);
		stream.Write(&shader_library.permutation_count, 4);

		for (uint32_t p = 0; p < shader_library.permutation_count; ++p)
		{
			ShaderData& permutation = shader_library.permutations[p];

			stream.Write(&permutation.name, 4);

			// Write resource reflection
			uint32_t rcount = (uint32_t)permutation.resource_reflection.resources.size();
			stream.Write(&rcount, 4);
			stream.Write(permutation.resource_reflection.resources.data(), rcount*sizeof(ShaderResourceReflection::Resource));

			// Constant buffer reflection
			uint32_t cbcount = (uint32_t)permutation.constant_buffer_reflections.size();
			stream.Write(&cbcount, 4);
			for (uint32_t i = 0; i < cbcount; ++i)
			{
				permutation.constant_buffer_reflections[i].Serialize(stream);
				// Write template data, if any
				if (permutation.constant_buffer_reflections[i].type != RConstantBuffer::TYPE_GLOBAL)
				{
					uint32_t bsize = permutation.constant_buffer_reflections[i].size;
					if (bsize > 0)
						stream.Write(permutation.constant_buffer_templates[i].data, bsize);

				}
			}

			// Instance data reflection
			permutation.instance_data_reflection.Serialize(stream);

			uint32_t pass_count = (uint32_t)permutation.passes.size();
			stream.Write(&pass_count, 4);
			for (uint32_t i = 0; i < pass_count; ++i)
			{
				stream.Write(&permutation.passes[i].technique, 4);
				stream.Write(&permutation.passes[i].instanced, 1);
			}

			// Write platform data
			uint32_t psize = (uint32_t)permutation.platform_data.size();
			stream.Write(&psize, 4);
			if (psize)
				stream.Write(permutation.platform_data.data(), psize);

		}
	}


	void shader_library_resource::Load(ResourceLoader::LoadContext& context)
	{
		uint32_t version;

		// Shader version
		context.file->Read(&version, 4);
		if (version != SHADER_LIBRARY_RESOURCE_VERSION)
		{
			logging::Error("Failed loading shader library: Wrong version, tried loading version %d, current version is %d.", version, SHADER_LIBRARY_RESOURCE_VERSION);
		}

		ShaderManager* shader_manager = (ShaderManager*)context.user_data;
		Assert(shader_manager);

		RenderResourceAllocator* render_resource_allocator = shader_manager->GetResourceAllocator();

		uint32_t permutation_count;
		context.file->Read(&permutation_count, 4);

		ShaderLibrary* shader_library = new ShaderLibrary();
		shader_library->permutation_count = permutation_count;

		shader_library->render_resources = new RShader[permutation_count];
		shader_library->permutations = new ShaderData[permutation_count];

		for (uint32_t p = 0; p < permutation_count; ++p)
		{
			ShaderData* data = &shader_library->permutations[p];

			// Shader name
			context.file->Read(&data->name, 4);

			// Read resource reflection
			uint32_t rc;
			context.file->Read(&rc, 4);
			if (rc)
			{
				data->resource_reflection.resources.resize(rc);
				context.file->Read(data->resource_reflection.resources.data(), sizeof(ShaderResourceReflection::Resource)*rc);
			}

			// Constant buffer reflection
			uint32_t cbcount;
			context.file->Read(&cbcount, 4);
			for (uint32_t i = 0; i < cbcount; ++i)
			{
				data->constant_buffer_reflections.push_back(ConstantBufferReflection());
				ConstantBufferReflection& reflection = data->constant_buffer_reflections.back();

				reflection.Deserialize(*context.file);

				if (reflection.type == RConstantBuffer::TYPE_GLOBAL)
				{
					// No template data for global constant buffers

					ConstantBufferTemplate buffer_template;
					buffer_template.render_resource = shader_manager->CreateGlobalConstantBuffer(reflection.name, reflection);
					buffer_template.data = nullptr; // No data for global buffers

					data->constant_buffer_templates.push_back(buffer_template);
				}
				else if (reflection.type == RConstantBuffer::TYPE_LOCAL)
				{
					void* bdata = 0;
					if (reflection.size)
					{
						bdata = memory::Malloc(reflection.size);
						context.file->Read(bdata, reflection.size);
					}

					ConstantBufferTemplate buffer_template;
					buffer_template.render_resource = RConstantBuffer(reflection.size, reflection.type);
					render_resource_allocator->AllocateConstantBuffer(buffer_template.render_resource, bdata);
					buffer_template.data = bdata;

					data->constant_buffer_templates.push_back(buffer_template);
				}
			}

			// Instance data reflection
			data->instance_data_reflection.Deserialize(*context.file);

			uint32_t pass_count;
			context.file->Read(&pass_count, 4);
			data->passes.resize(pass_count);
			for (uint32_t i = 0; i < pass_count; ++i)
			{
				context.file->Read(&data->passes[i].technique, 4);
				context.file->Read(&data->passes[i].instanced, 1);
			}

			// Platform data
			uint32_t psize = 0;
			context.file->Read(&psize, 4);
			if (psize)
			{
				data->platform_data.resize(psize);
				context.file->Read(data->platform_data.data(), psize);
			}

			shader_library->render_resources[p] = RShader(data->platform_data.data(), (uint32_t)data->platform_data.size());
			render_resource_allocator->AllocateShader(shader_library->render_resources[p]);

		}


		context.result = shader_library;
	}
	void shader_library_resource::Unload(ResourceLoader::UnloadContext& context)
	{
		ShaderManager* shader_manager = (ShaderManager*)context.user_data;
		RenderResourceAllocator* render_resource_allocator = shader_manager->GetResourceAllocator();

		ShaderLibrary* shader_library = (ShaderLibrary*)context.resource_data;

		for (uint32_t p = 0; p < shader_library->permutation_count; ++p)
		{
			ShaderData& permutation = shader_library->permutations[p];

			render_resource_allocator->ReleaseResource(shader_library->render_resources[p]);

			// Free all constant buffer templates
			for (uint32_t i = 0; i < permutation.constant_buffer_reflections.size(); ++i)
			{
				ConstantBufferReflection& buffer_reflection = permutation.constant_buffer_reflections[i];
				ConstantBufferTemplate& buffer_template = permutation.constant_buffer_templates[i];
				if (buffer_reflection.type == RConstantBuffer::TYPE_GLOBAL)
				{
					shader_manager->ReleaseGlobalConstantBuffer(buffer_reflection.name);
				}
				else if (buffer_reflection.type == RConstantBuffer::TYPE_LOCAL)
				{
					render_resource_allocator->ReleaseResource(buffer_template.render_resource);
					memory::Free(buffer_template.data);
				}
			}
		}

		delete[] shader_library->permutations;
		delete[] shader_library->render_resources;

		delete shader_library;
	}
	void shader_library_resource::BringIn(void* user_data, void* resource_data)
	{
		ShaderManager* shader_manager = (ShaderManager*)user_data;
		ShaderLibrary* shader_library = (ShaderLibrary*)resource_data;

		shader_manager->AddLibrary(shader_library);
	}
	void shader_library_resource::BringOut(void* user_data, void* resource_data)
	{
		ShaderManager* shader_manager = (ShaderManager*)user_data;
		ShaderLibrary* shader_library = (ShaderLibrary*)resource_data;

		shader_manager->RemoveLibrary(shader_library);
	}

	//-------------------------------------------------------------------------------

	void shader_library_resource::RegisterResourceType(ResourceManager* resource_manager, ShaderManager* shader_manager)
	{
		ResourceType resource_type;
		resource_type.user_data = (void*)shader_manager;
		resource_type.load_callback = Load;
		resource_type.unload_callback = Unload;
		resource_type.bring_in_callback = BringIn;
		resource_type.bring_out_callback = BringOut;

		resource_manager->RegisterType("shader", resource_type);
	}
	void shader_library_resource::UnregisterResourceType(ResourceManager* resource_manager)
	{
		resource_manager->UnregisterType("shader");
	}

	//-------------------------------------------------------------------------------

} // namespace sb
