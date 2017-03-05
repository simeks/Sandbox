// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_RESOURCEMANAGER_H__
#define __FOUNDATION_RESOURCEMANAGER_H__

#include "ResourceLoader.h"



namespace sb
{

	/// @brief Contains callback and user data needed for resource loading of a specific resource type.
	struct ResourceType
	{
		typedef void(*LoadFn)(ResourceLoader::LoadContext&);
		typedef void(*UnloadFn)(ResourceLoader::UnloadContext&);
		typedef void(*BringInFn)(void*, void*);				// void* user_data, void* resource_data
		typedef void(*BringOutFn)(void*, void*);			// void* user_data, void* resource_data

		ResourceType()
			: user_data(nullptr),
			load_callback(nullptr),
			unload_callback(nullptr),
			bring_in_callback(nullptr),
			bring_out_callback(nullptr)
		{
		}

		void*		user_data;

		/// Load function: Called on a separate thread for loading a resource
		LoadFn		load_callback;
		/// Unload function: Called on unload, should reverse the effect of load_callback
		UnloadFn	unload_callback;

		/// Bring-in callback: Called on main-thread when resource is added to the resource manager
		BringInFn	bring_in_callback;
		/// Bring-out callback: Called on main-thread when resource is removed from the resource manager, should reverse the effect of bring_in_callback
		BringOutFn	bring_out_callback;
	};

	class ResourceManager
	{
	public:
		struct ResourceData
		{
			ResourceData() : data(0), ref_count(0) {}
			ResourceData(void* _data) : data(_data), ref_count(0) {}

			void* data;

			/// Every resource is reference counted to allow the same resource to be loaded/unloaded multiple times
			///		without risking unloading it before all users are done with it.
			uint32_t ref_count;
		};


	public:
		ResourceManager(FileSystem* file_system);
		~ResourceManager();


		void Initialize();
		void Shutdown();

		/// @brief Loads a resource
		/// @param resource_type Resource type (e.g. "material")
		/// @param resource_name Resource name (e.g. "materials/floor")
		/// @param source File source to load resource from, if NULL the loader will try to 
		///					load the resource from the applications base path.
		/// This is a non-blocking call that will send a load request to the resource loader.
		///		Which means that the resource aren't ready directly after this call. If you 
		///		want to block until the resource is loaded use Flush().
		void Load(const char* resource_type, const char* resource_name, FileSource* source);

		/// @brief Tries to cancel the loading of a resource
		///	@return True if cancellation was successful 
		bool CancelLoad(const StringId64& type_id, const StringId64& resource_id);

		void Unload(const StringId64& type_id, const StringId64& resource_id);
		void UnloadAll();

		/// @brief Stalls this thread until all queued resource are loaded
		void Flush();

		/// @brief Stalls this thread until all resources queued before the marker are loaded
		void Flush(uint32_t marker);

		/// @brief Brings in all completed resources
		///
		///	Brings in all completed resources and puts them in the managers resource map,
		///		making them available for use.
		void BringIn();

		/// Pushes a marker onto the queue, this way a package can keep track if all its resources
		///		has been loaded.
		uint32_t PushMarker();

		/// Checks if the specified marker has been loaded.
		bool IsLoaded(uint32_t marker) const;


		/// @return True if the manager has the specified resource
		bool HasResource(const StringId64& type_id, const StringId64& resource_id);

		/// @brief Returns the resoruce data for the specifed resource
		void* GetResource(const StringId64& type_id, const StringId64& resource_id);

		void RegisterType(StringId64 type_id, const ResourceType& type);
		void UnregisterType(StringId64 type_id);
		void UnregisterAllTypes();

		bool HasType(StringId64 type_id);

	private:
		/// Internal struct for request in the manager
		struct ResourceRequest
		{
			StringId64 type_id;
			StringId64 resource_id;
			LoadRequestId request_id;
			bool marker;

			ResourceRequest() : marker(false) {}
		};

		typedef map<StringId64, ResourceType> ResourceTypeMap;
		typedef pair<StringId64, StringId64> ResourceId; // <Type ID, Resource ID>
		typedef map<ResourceId, ResourceData> ResourceDataMap; // <Type ID, Resource ID> => Resource data
		typedef deque<ResourceRequest> LoadRequestQueue;

		FileSystem*		_file_system;

		ResourceTypeMap _resource_types;
		ResourceDataMap _resource_data;

		LoadRequestQueue _load_requests;
		uint32_t _queued_markers;
		uint32_t _loaded_markers;

		ResourceLoader* _resource_loader;

		/// @brief Finalizes a request
		///	@param result Result from the ResourceLoader
		void FinalizeRequest(const ResourceRequest& request, const ResourceLoader::Result& result);

	};

} // namespace sb



#endif // __FOUNDATION_RESOURCEMANAGER_H__