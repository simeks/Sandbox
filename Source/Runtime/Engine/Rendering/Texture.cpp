// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Texture.h"
#include <Engine/Rendering/RTexture.h>
#include <Engine/Rendering/DDSImage.h>
#include <Engine/Rendering/RenderDevice.h>
#include <Engine/Rendering/RenderResourceAllocator.h>

#include <Foundation/Filesystem/File.h>

namespace sb
{

	Texture::Texture()
	{
	}
	Texture::~Texture()
	{
	}

	void Texture::Load(FileStreamPtr file, RenderResourceAllocator* resource_allocator)
	{
		// Load dds file
		dds_image::Load(*file, _render_resource.GetDesc(), _surfaces);

		// Allocate render resource
		resource_allocator->AllocateTexture(_render_resource, &_surfaces);
	}

	void Texture::Unload(RenderResourceAllocator* resource_allocator)
	{
		// Release render resource
		resource_allocator->ReleaseResource(_render_resource);

		// Free allocated memory
		for (auto& surface : _surfaces)
		{
			memory::Free(surface.data);
		}
	}

	const RTexture& Texture::GetRenderResource() const
	{
		return _render_resource;
	}


	//-------------------------------------------------------------------------------

	void texture_resource::Load(ResourceLoader::LoadContext& context)
	{
		Texture* texture = new Texture();

		RenderDevice* render_device = (RenderDevice*)context.user_data;
		texture->Load(context.file, render_device->GetResourceAllocator());

		context.result = texture;
	}
	void texture_resource::Unload(ResourceLoader::UnloadContext& context)
	{
		Texture* texture = (Texture*)context.resource_data;

		RenderDevice* render_device = (RenderDevice*)context.user_data;
		texture->Unload(render_device->GetResourceAllocator());

		delete texture;
	}

	void texture_resource::RegisterResourceType(ResourceManager* resource_manager, RenderDevice* render_device)
	{
		ResourceType resource_type;
		resource_type.load_callback = Load;
		resource_type.unload_callback = Unload;
		resource_type.user_data = render_device;

		resource_manager->RegisterType("texture", resource_type);
	}
	void texture_resource::UnregisterResourceType(ResourceManager* resource_manager)
	{
		resource_manager->UnregisterType("texture");
	}

} // namespace sb
