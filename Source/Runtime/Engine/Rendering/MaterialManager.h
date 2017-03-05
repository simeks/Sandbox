// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_MATERIALMANAGER_H__
#define __RENDERING_MATERIALMANAGER_H__

namespace sb
{

	class ResourceManager;
	class ShaderManager;
	class Material;

	class MaterialManager
	{
	public:
		MaterialManager(ShaderManager* shader_manager, ResourceManager* resource_manager);
		~MaterialManager();

		/// @return Material with the specified name, NULL if no material was found
		Material* GetMaterial(StringId32 name);


		/// Adds a new material to the manager
		void AddMaterial(Material* material);

		/// Removes a material from the manager
		void RemoveMaterial(StringId32 name);

	private:
		ResourceManager* _resource_manager;
		ShaderManager* _shader_manager;

		map<StringId32, Material*> _materials;

	};

} // namespace sb

#endif // __RENDERING_MATERIALMANAGER_H__
