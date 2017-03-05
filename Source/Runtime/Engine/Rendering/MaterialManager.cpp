// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "MaterialManager.h"
#include "Material.h"

namespace sb
{

	MaterialManager::MaterialManager(ShaderManager* shader_manager, ResourceManager* resource_manager)
		: _resource_manager(resource_manager),
		_shader_manager(shader_manager)
	{
		material_resource::RegisterResourceType(_resource_manager, this);
	}
	MaterialManager::~MaterialManager()
	{
		material_resource::UnregisterResourceType(_resource_manager);
	}

	Material* MaterialManager::GetMaterial(StringId32 name)
	{
		map<StringId32, Material*>::iterator it = _materials.find(name);
		if (it == _materials.end())
			return nullptr; // Not found

		// Initialize if not initialized
		if (!it->second->Initialized())
			it->second->Initialize(_shader_manager, _resource_manager);

		return it->second;
	}

	void MaterialManager::AddMaterial(Material* material)
	{
		StringId32 material_name = material->GetName();

		map<StringId32, Material*>::iterator it = _materials.find(material_name);
		Assert(it == _materials.end()); // Material with that name already exists in manager

		_materials[material_name] = material;
	}
	void MaterialManager::RemoveMaterial(StringId32 name)
	{
		map<StringId32, Material*>::iterator it = _materials.find(name);
		if (it != _materials.end())
		{
			_materials.erase(it);
		}
	}

} // namespace sb
