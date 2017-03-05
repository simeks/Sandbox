// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_TEXTURE_H__
#define __RENDERING_TEXTURE_H__

#include <Engine/Rendering/RTexture.h>
#include <Foundation/Resource/ResourceManager.h>

namespace sb
{

	class RenderDevice;

	class Texture
	{
	public:
		Texture();
		~Texture();

		/// @brief Loads the texture from the specified file stream.
		void Load(FileStreamPtr file, RenderResourceAllocator* resource_allocator);

		/// @brief Unloads the texture, frees any allocated memory and releases render resources.
		void Unload(RenderResourceAllocator* resource_allocator);

		/// @brief Returns the textures render resource.
		const RTexture& GetRenderResource() const;

	private:
		RTexture _render_resource;
		vector<image::Surface> _surfaces;

	};

	namespace texture_resource
	{
		enum { TEXTURE_RESOURCE_VERSION = 2 };

		void Load(ResourceLoader::LoadContext& context);
		void Unload(ResourceLoader::UnloadContext& context);

		void RegisterResourceType(ResourceManager* resource_manager, RenderDevice* render_device);
		void UnregisterResourceType(ResourceManager* resource_manager);

	};

} // namespace sb

#endif // __RENDERING_TEXTURE_H__
