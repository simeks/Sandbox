// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_SHADERLIBRARY_H__
#define __RENDERING_SHADERLIBRARY_H__

#include <Foundation/Resource/ResourceManager.h>

namespace sb
{

	struct ShaderData;

	class RShader;
	class ShaderManager;

	struct ShaderLibrary
	{
		uint32_t permutation_count;
		ShaderData* permutations;

		RShader* render_resources; // Handles to allocated resources

		ShaderLibrary() : permutation_count(0), permutations(nullptr), render_resources(nullptr) {}
	};

	//-------------------------------------------------------------------------------

	namespace shader_library_resource
	{
		enum { SHADER_LIBRARY_RESOURCE_VERSION = 8 };

		void Compile(const ShaderLibrary& shader_library, Stream& stream);

		void Load(ResourceLoader::LoadContext& context);
		void Unload(ResourceLoader::UnloadContext& context);
		void BringIn(void* user_data, void* resource_data);
		void BringOut(void* user_data, void* resource_data);


		void RegisterResourceType(ResourceManager* resource_manager, ShaderManager* shader_manager);
		void UnregisterResourceType(ResourceManager* resource_manager);

	};

} // namespace sb

#endif // __RENDERING_SHADERLIBRARY_H__

