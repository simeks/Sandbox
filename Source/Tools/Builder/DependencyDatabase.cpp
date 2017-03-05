// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "DependencyDatabase.h"

#include <Foundation/Filesystem/FilePath.h>
#include <Foundation/Filesystem/FileSource.h>
#include <Foundation/Container/ConfigValue.h>
#include <Foundation/Json/Json.h>


namespace sb
{

	//-------------------------------------------------------------------------------
	DependencyDatabase::DependencyDatabase(FileSource* file_source)
		: _file_source(file_source)
	{
	}
	DependencyDatabase::~DependencyDatabase()
	{
	}
	//-------------------------------------------------------------------------------
	void DependencyDatabase::AddDependent(const char* resource_path, const char* dependent_path)
	{
		FilePath res_file_path(resource_path);
		res_file_path.SetSeparator('/');

		DependencyMap::iterator map_it = _dependencies.find(res_file_path.Get());
		if (map_it == _dependencies.end())
		{
			// Make sure the entry gets constructed with the right allocator
			map_it = _dependencies.insert(pair<string, Entry>(res_file_path.Get(), Entry())).first;
		}
		Entry& entry = map_it->second;

		FilePath dep_file_path(dependent_path);
		dep_file_path.SetSeparator('/');

		// Check if it already is in there
		vector<string>::iterator it, end;
		it = entry.dependents.begin(); end = entry.dependents.end();
		for (; it != end; ++it)
		{
			if ((*it) == dep_file_path.Get())
				return;
		}
		entry.dependents.push_back(dep_file_path.Get());
	}
	void DependencyDatabase::GetDependents(const char* resource_path, vector<string>& dependents)
	{
		FilePath res_file_path(resource_path);
		res_file_path.SetSeparator('/');

		DependencyMap::iterator map_it = _dependencies.find(res_file_path.Get());
		if (map_it == _dependencies.end())
		{
			return;
		}
		Entry& entry = map_it->second;
		dependents = entry.dependents;
	}
	void DependencyDatabase::Clear()
	{
		_dependencies.clear();
	}
	//-------------------------------------------------------------------------------
	void DependencyDatabase::UpdateModifiedTime(const char* resource_path, const FileTime& modified_time)
	{
		FilePath res_file_path(resource_path);
		res_file_path.SetSeparator('/');

		DependencyMap::iterator map_it = _dependencies.find(res_file_path.Get());
		if (map_it == _dependencies.end())
		{
			// Make sure the entry gets constructed with the right allocator
			map_it = _dependencies.insert(pair<string, Entry>(res_file_path.Get(), Entry())).first;
		}
		map_it->second.last_modified = modified_time;
	}
	bool DependencyDatabase::GetModifiedTime(const char* resource_path, FileTime& modified_time)
	{
		FilePath res_file_path(resource_path);
		res_file_path.SetSeparator('/');

		DependencyMap::iterator map_it = _dependencies.find(res_file_path.Get());
		if (map_it == _dependencies.end())
		{
			return false;
		}
		modified_time = map_it->second.last_modified;
		return true;
	}
	//-------------------------------------------------------------------------------
	void DependencyDatabase::Save()
	{
		Assert(_file_source);

		ConfigValue root;
		root.SetEmptyObject();


		for (auto& dep_list : _dependencies)
		{
			FileTime file_time;

			if (!_file_source->LastModifiedTime(dep_list.first.c_str(), file_time))
				continue;

			ConfigValue& entry = root[dep_list.first.c_str()];
			entry.SetEmptyObject();

			entry["time"].SetEmptyArray();
			entry["time"].Append().SetUInt(file_time.high);
			entry["time"].Append().SetUInt(file_time.low);

			ConfigValue& dependents = entry["dependents"];
			dependents.SetEmptyArray();

			for (auto& dep : dep_list.second.dependents)
			{
				dependents.Append().SetString(dep.c_str());
			}
		}

		// Make sure .builder folder exists
		_file_source->MakeDirectory(".builder");

		FileStreamPtr file = _file_source->OpenFile(".builder/dependency_database", File::WRITE);
		if (!file.Get())
		{
			logging::Warning("Failed to save dependency database");
			return;
		}

		json::Writer writer;
		stringstream ss;
		writer.Write(root, ss, true);

		file->Write(ss.str().c_str(), ss.str().size());

		logging::Info("Dependency database successfully saved.");
	}
	bool DependencyDatabase::Load()
	{
		Assert(_file_source);
		ConfigValue root;

		json::Reader reader;
		if (!reader.ReadFile(_file_source, ".builder/dependency_database", root))
		{
			logging::Warning("Failed loading dependency database: %s", reader.GetErrorMessage().c_str());
			return false;
		}

		ConfigValue::Iterator it, end;
		it = root.Begin(); end = root.End();
		for (; it != end; ++it)
		{
			FileTime last_time;
			FileTime cur_time;
			ConfigValue& entry = it->second;

			Assert(entry.IsObject());
			if (!entry.IsObject())
				continue;

			Assert(entry["time"].IsArray());
			Assert(entry["time"].Size() == 2);
			last_time.high = entry["time"][0].AsUInt();
			last_time.low = entry["time"][1].AsUInt();

			Assert(entry["dependents"].IsArray());
			ConfigValue& dependents = entry["dependents"];
			for (uint32_t i = 0; i < dependents.Size(); ++i)
			{
				Assert(dependents[i].IsString());
				if (!dependents[i].IsString())
					continue;

				AddDependent(it->first.c_str(), dependents[i].AsString());
			}

			UpdateModifiedTime(it->first.c_str(), last_time);
		}

		logging::Info("Dependency database successfully loaded.");
		return true;
	}
	//-------------------------------------------------------------------------------

} // namespace sb



