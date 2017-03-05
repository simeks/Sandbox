// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_RESOURCEPACKAGE_H__
#define __FOUNDATION_RESOURCEPACKAGE_H__

#include "ResourceManager.h"

namespace sb
{

	class Stream;
	class ResourcePackage
	{
	public:
		enum PackageType
		{
			DIRECTORY_PACKAGE, ///< Package where all resources are just separate files
		};
		enum State { UNLOADED, QUEUED, LOADED };

		ResourcePackage(ResourceManager* resource_manager);
		~ResourcePackage();

		void Load();
		void Unload();

		/// Returns true if the package has completed loading
		bool IsLoaded() const;

		/// Flushes the package, if the package hasn't completed loading yet, then this method will block until
		///		all resources are loaded.
		void Flush();

		void AddResource(const char* resource_type, const char* resource_name);

		/// Returns the current state of the package.
		State GetState() const;

	private:
		ResourcePackage& operator=(const ResourcePackage& other);

		struct Resource
		{
			Resource(const char* resource_type, const char* resource_name)
				: type(resource_type), name(resource_name)
			{}

			string type;
			string name;
		};

		ResourceManager* _resource_manager;
		uint32_t _load_marker;
		State _state;

		PackageType _package_type;
		vector<Resource> _resources;
	};

	class ConfigValue;
	namespace package_resource
	{
		enum { PACKAGE_RESOURCE_VERSION = 2 };

		/// Compiles a resource package from a config file
		void Compile(const ConfigValue& root, Stream& stream);

		void Load(ResourceLoader::LoadContext& context);
		void Unload(ResourceLoader::UnloadContext& context);
		void BringIn(void* user_data, void* resource_data);
		void BringOut(void* user_data, void* resource_data);

		void RegisterResourceType(ResourceManager* resource_manager);
		void UnregisterResourceType(ResourceManager* resource_manager);
	};

} // namespace sb


#endif // __FOUNDATION_RESOURCEPACKAGE_H__

