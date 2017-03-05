// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ResourcePackage.h"
#include "Filesystem/File.h"
#include "IO/MemoryStream.h"
#include "Container/ConfigValue.h"


namespace sb
{

	//-------------------------------------------------------------------------------
	ResourcePackage::ResourcePackage(ResourceManager* resource_manager)
		: _resource_manager(resource_manager),
		_load_marker(Invalid<uint32_t>()),
		_state(UNLOADED),
		_package_type(DIRECTORY_PACKAGE)
	{

	}
	ResourcePackage::~ResourcePackage()
	{
		Assert(_state == UNLOADED);
	}
	void ResourcePackage::Load()
	{
		for (auto& resource : _resources)
		{
			_resource_manager->Load(resource.type.c_str(), resource.name.c_str(), nullptr);
		}
		_load_marker = _resource_manager->PushMarker();
		_state = QUEUED;
	}
	void ResourcePackage::Unload()
	{
		for (auto& resource : _resources)
		{
			_resource_manager->Unload(resource.type, resource.name);
		}
		_state = UNLOADED;
	}
	bool ResourcePackage::IsLoaded() const
	{
		_resource_manager->BringIn();
		return _resource_manager->IsLoaded(_load_marker);
	}
	void ResourcePackage::Flush()
	{
		_resource_manager->Flush(_load_marker);
		_state = LOADED;
	}
	void ResourcePackage::AddResource(const char* resource_type, const char* resource_name)
	{
		_resources.push_back(Resource(resource_type, resource_name));
	}
	ResourcePackage::State ResourcePackage::GetState() const
	{
		return _state;
	}

	//-------------------------------------------------------------------------------


	//-------------------------------------------------------------------------------
	void package_resource::Compile(const ConfigValue& root, Stream& stream)
	{
		uint32_t version = PACKAGE_RESOURCE_VERSION;
		uint8_t type = ResourcePackage::DIRECTORY_PACKAGE;

		stream.Write(&version, 4);
		stream.Write(&type, 1);

		const ConfigValue& resources = root["resources"];
		Assert(resources.IsArray());

		uint32_t count = resources.Size();
		stream.Write(&count, 4);
		for (uint32_t i = 0; i < count; ++i)
		{
			Assert(resources[i].IsArray() && resources[i].Size() == 2);
			Assert(resources[i][0].IsString() && resources[i][1].IsString());
			uint64_t id = StringId64(resources[i][1].AsString()).GetId();

			stream.Write(&id, sizeof(uint64_t));
			stream.Write(resources[i][0].AsString(), resources[i][0].Size() + 1);
			stream.Write(resources[i][1].AsString(), resources[i][1].Size() + 1);
		}
	}

	void package_resource::Load(ResourceLoader::LoadContext& context)
	{
		ResourcePackage* package = new ResourcePackage((ResourceManager*)context.user_data);
		Assert(package);
		context.result = package;

		uint32_t version;
		uint8_t package_type;
		uint32_t count;
		context.file->Read(&version, 4);
		if (version != PACKAGE_RESOURCE_VERSION)
		{
			logging::Error("Failed loading package: Wrong version, tried loading version %d, current version is %d.", version, PACKAGE_RESOURCE_VERSION);
		}

		context.file->Read(&package_type, 1);
		context.file->Read(&count, 4);

		Assert(package_type == ResourcePackage::DIRECTORY_PACKAGE);

		string type = "";
		string name = "";
		for (uint32_t i = 0; i < count; ++i)
		{
			uint64_t id;
			context.file->Read(&id, sizeof(uint64_t));

			char c;
			while (context.file->Read(&c, 1))
			{
				if (c == '\0')
					break;
				type += c;
			}
			while (context.file->Read(&c, 1))
			{
				if (c == '\0')
					break;
				name += c;
			}

			package->AddResource(type.c_str(), name.c_str());

			type.clear();
			name.clear();
		}
	}
	void package_resource::Unload(ResourceLoader::UnloadContext& context)
	{
		ResourcePackage* package = (ResourcePackage*)context.resource_data;
		delete package;
	}

	void package_resource::RegisterResourceType(ResourceManager* resource_manager)
	{
		ResourceType resource_type;
		resource_type.user_data = resource_manager;
		resource_type.load_callback = Load;
		resource_type.unload_callback = Unload;
		resource_manager->RegisterType("package", resource_type);
	}
	void package_resource::UnregisterResourceType(ResourceManager* resource_manager)
	{
		resource_manager->UnregisterType("package");
	}
	//-------------------------------------------------------------------------------

} // namespace sb

