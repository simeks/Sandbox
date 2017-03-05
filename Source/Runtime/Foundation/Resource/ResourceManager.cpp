// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Memory/Memory.h"
#include "ResourceManager.h"
#include "ResourcePackage.h"
#include "Filesystem/FileSystem.h"


namespace sb
{

	//-------------------------------------------------------------------------------
	ResourceManager::ResourceManager(FileSystem* file_system)
		: _file_system(file_system),
		_queued_markers(0),
		_loaded_markers(0)
	{
		_resource_loader = new ResourceLoader(_file_system);

	}
	ResourceManager::~ResourceManager()
	{
		delete _resource_loader;
	}
	//-------------------------------------------------------------------------------
	void ResourceManager::Initialize()
	{
		_resource_loader->Initialize();

		package_resource::RegisterResourceType(this);
	}
	void ResourceManager::Shutdown()
	{
		package_resource::UnregisterResourceType(this);

		ResourceLoader::Result result;
		// Clean all pending requests
		for (int i = (uint32_t)_load_requests.size() - 1; i >= 0; --i)
		{
			if (!_resource_loader->CancelRequest(_load_requests[i].request_id))
			{
				// Cancel not successful so either request is processed or currently processing
				if (_resource_loader->WaitResult(_load_requests[i].request_id, result))
				{
					FinalizeRequest(_load_requests[i], result);
				}
			}
		}
		_load_requests.clear();

		UnloadAll();
		_resource_loader->Shutdown();

	}
	//-------------------------------------------------------------------------------
	void ResourceManager::Load(const char* resource_type, const char* resource_name, FileSource* source)
	{
		StringId64 type_id = resource_type, resource_id = resource_name;

		ResourceDataMap::iterator data_it = _resource_data.find(ResourceId(type_id, resource_id));

		// Check if resource is already loaded
		if (data_it != _resource_data.end())
		{
			// Resource is already loaded so we just increase its reference count.
			++data_it->second.ref_count;

			return;
		}

		ResourceTypeMap::iterator type_it = _resource_types.find(type_id);
		Assert(type_it != _resource_types.end());

		ResourceType& type = type_it->second;

		// Fill in our internal request structure so we can push it into our queue later
		ResourceRequest internal_request;
		internal_request.resource_id = resource_name;
		internal_request.type_id = type_id;

		ResourceLoader::Request request;
		request.load_callback = type.load_callback;
		request.user_data = type.user_data;

		// Resource path = {Resource name}.{Resource type} (e.g. "materials/floor.material")
		stringstream ss; ss << resource_name << "." << resource_type;
		request.resource_path = ss.str();
		request.file_source = source;
		request.result = 0;

		// Add the request to the loader
		internal_request.request_id = _resource_loader->AddRequest(request);

		// Push request to queue
		_load_requests.push_back(internal_request);

		// Create a resource entry now so we still can keep track of the reference count while resource is queued
		ResourceData resource_data;
		resource_data.data = nullptr;
		resource_data.ref_count = 1;
		_resource_data[ResourceId(type_id, resource_id)] = resource_data;

		logging::Info("ResourceManager: Resource %s.%s (0x%llx) queued for loading.", resource_name, resource_type, resource_id.GetId());
	}
	bool ResourceManager::CancelLoad(const StringId64& type_id, const StringId64& resource_id)
	{
		// Try to find the request
		for (int i = (uint32_t)_load_requests.size() - 1; i >= 0; --i)
		{
			if (_load_requests[i].type_id == type_id && _load_requests[i].resource_id == resource_id)
			{
				return _resource_loader->CancelRequest(_load_requests[i].request_id);
			}
		}
		// Not found
		return false;
	}

	void ResourceManager::Unload(const StringId64& type_id, const StringId64& resource_id)
	{
		ResourceDataMap::iterator data_it = _resource_data.find(ResourceId(type_id, resource_id));
		Assert(data_it != _resource_data.end());

		ResourceData& data = data_it->second;
		if (--data.ref_count != 0) // There's still someone using this resource
			return;

		void* resource_data = data.data;
		Assert(resource_data != nullptr); // Resource isn't fully loaded

		ResourceType& type = _resource_types[type_id];

		// Call bring-out
		if (type.bring_out_callback)
			type.bring_out_callback(type.user_data, resource_data);

		// Remove resource data from map
		_resource_data.erase(data_it);

		ResourceLoader::UnloadContext context;
		context.user_data = type.user_data;
		context.resource_data = resource_data;

		// Call unload
		if (type.unload_callback)
			type.unload_callback(context);

	}
	void ResourceManager::UnloadAll()
	{
		ResourceDataMap::iterator data_it = _resource_data.begin(), data_end = _resource_data.end();

		for (; data_it != data_end; ++data_it)
		{
			ResourceData& data = data_it->second;
			if (--data.ref_count != 0) // There's still someone using this resource
				return;

			void* resource_data = data.data;
			Assert(resource_data != nullptr); // Resource isn't fully loaded

			ResourceType& type = _resource_types[data_it->first.first];

			// Call bring-out
			if (type.bring_out_callback)
				type.bring_out_callback(type.user_data, resource_data);

			// Remove resource data from map
			_resource_data.erase(data_it);

			ResourceLoader::UnloadContext context;
			context.user_data = type.user_data;
			context.resource_data = resource_data;

			// Call unload
			if (type.unload_callback)
				type.unload_callback(context);

		}

	}
	void ResourceManager::BringIn()
	{
		ResourceLoader::Result result;
		while (!_load_requests.empty())
		{
			ResourceRequest& req = _load_requests.front();
			if (_resource_loader->GetResult(req.request_id, result))
			{
				FinalizeRequest(req, result);
				_load_requests.pop_front();
			}
			else
			{
				// The requests order are the same here as in the loader, so if a request
				//	in the queue isn't completed then all requests behind are also pending
				break;
			}
		}
	}
	void ResourceManager::Flush()
	{
		ResourceLoader::Result result;
		while (!_load_requests.empty())
		{
			ResourceRequest& req = _load_requests.front();
			if (_resource_loader->WaitResult(req.request_id, result))
			{
				FinalizeRequest(req, result);
				_load_requests.pop_front();
			}
			else
			{
				// Something went wrong here
				Assert(false);
				return;
			}
		}
	}
	void ResourceManager::Flush(uint32_t marker)
	{
		ResourceLoader::Result result;
		while (!IsLoaded(marker))
		{
			ResourceRequest& req = _load_requests.front();
			if (_resource_loader->WaitResult(req.request_id, result))
			{
				FinalizeRequest(req, result);
				_load_requests.pop_front();
			}
			else
			{
				// Something went wrong here
				Assert(false);
				return;
			}
		}
	}

	uint32_t ResourceManager::PushMarker()
	{
		uint32_t marker = _queued_markers++;

		ResourceLoader::Request request;
		request.load_callback = nullptr;
		request.user_data = nullptr;
		request.file_source = nullptr;
		request.result = 0;

		// Fill in our internal request structure so we can push it into our queue later
		ResourceRequest internal_request;
		// Add the request to the loader
		internal_request.request_id = _resource_loader->AddRequest(request);
		internal_request.marker = true;

		// Push request to queue
		_load_requests.push_back(internal_request);

		return marker;
	}
	bool ResourceManager::IsLoaded(uint32_t marker) const
	{
		if (marker < _loaded_markers)
			return true;
		return false;
	}

	//-------------------------------------------------------------------------------
	bool ResourceManager::HasResource(const StringId64& type_id, const StringId64& resource_id)
	{
		ResourceDataMap::const_iterator it = _resource_data.find(ResourceId(type_id, resource_id));
		if (it == _resource_data.end())
		{
			return false;
		}

		// If the data pointer is still NULL, the resource is still queued.
		if (it->second.data == nullptr)
		{
			return false;
		}

		return true;
	}
	void* ResourceManager::GetResource(const StringId64& type_id, const StringId64& resource_id)
	{
		ResourceDataMap::const_iterator it = _resource_data.find(ResourceId(type_id, resource_id));
		Assert(it != _resource_data.end());

		const ResourceData& data = it->second;
		// If the data pointer is still NULL, the resource is still queued.
		Assert(data.data != nullptr);

		return data.data;
	}
	//-------------------------------------------------------------------------------
	void ResourceManager::RegisterType(StringId64 type_id, const ResourceType& type)
	{
		ResourceTypeMap::const_iterator iter = _resource_types.find(type_id);
		if (iter != _resource_types.end())
			return; // Already exists

		logging::Info("ResourceManager: Registering resource type 0x%llx", type_id.GetId());

		_resource_types[type_id] = type;
	}
	void ResourceManager::UnregisterType(StringId64 type_id)
	{
		ResourceTypeMap::iterator iter = _resource_types.find(type_id);
		if (iter != _resource_types.end())
		{
			logging::Info("ResourceManager: Unregistering resource type 0x%llx", type_id.GetId());
			_resource_types.erase(iter);
		}
	}
	void ResourceManager::UnregisterAllTypes()
	{
		_resource_types.clear();
	}
	bool ResourceManager::HasType(StringId64 type_id)
	{
		ResourceTypeMap::const_iterator iter = _resource_types.find(type_id);
		if (iter != _resource_types.end())
			return true;
		return false;
	}
	//-------------------------------------------------------------------------------
	void ResourceManager::FinalizeRequest(const ResourceRequest& request, const ResourceLoader::Result& result)
	{
		if (request.marker)
		{
			_loaded_markers++;
		}
		else
		{
			Assert(result.result != 0);
			if (result.result == 0)
			{
				logging::Error("ResourceManager::FinalizeRequest: Result is NULL");
				return;
			}

			// Resource entry was created when the request was made, we only need to its data
			ResourceDataMap::iterator data_it = _resource_data.find(ResourceId(request.type_id, request.resource_id));
			Assert(data_it != _resource_data.end());

			data_it->second.data = result.result;

			ResourceTypeMap::iterator type_it = _resource_types.find(request.type_id);
			Assert(type_it != _resource_types.end());

			// Call bring-in function
			if (type_it->second.bring_in_callback)
				type_it->second.bring_in_callback(type_it->second.user_data, result.result);
		}
	}
	//-------------------------------------------------------------------------------

} // namespace sb

