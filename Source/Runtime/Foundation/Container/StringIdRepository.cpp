// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "StringIdRepository.h"
#include "ConfigValue.h"
#include "Filesystem/FileSource.h"
#include "Json/Json.h"

namespace sb
{

	StringIdRepository::StringIdRepository(FileSource* file_source, const char* target_path)
		: _file_source(file_source),
		_target_path(target_path)
	{
	}
	StringIdRepository::~StringIdRepository()
	{
	}

	void StringIdRepository::Add(uint32_t id, const char* str, uint32_t len)
	{
		map<uint32_t, string>::const_iterator it = _strings_32.find(id);
		if (it == _strings_32.end())
			_strings_32[id] = string(str, len);
	}
	void StringIdRepository::Add(uint64_t id, const char* str, uint32_t len)
	{
		map<uint64_t, string>::const_iterator it = _strings_64.find(id);
		if (it == _strings_64.end())
			_strings_64[id] = string(str, len);
	}

	const char* StringIdRepository::LookUp(uint32_t id) const
	{
		map<uint32_t, string>::const_iterator it = _strings_32.find(id);
		if (it != _strings_32.end())
			return it->second.c_str();

		return nullptr;
	}

	const char* StringIdRepository::LookUp(uint64_t id) const
	{
		map<uint64_t, string>::const_iterator it = _strings_64.find(id);
		if (it != _strings_64.end())
			return it->second.c_str();

		return nullptr;
	}

	void StringIdRepository::Save()
	{
		Assert(_file_source);
		Assert(!_target_path.empty());

		ConfigValue root;
		root.SetEmptyObject();

		root["string_id_32"].SetEmptyArray();
		for (auto& id : _strings_32)
		{
			ConfigValue& entry = root["string_id_32"].Append();
			entry.SetEmptyArray();
			entry.Append().SetUInt(id.first);
			entry.Append().SetString(id.second.c_str());
		}


		root["string_id_64"].SetEmptyArray();
		for (auto& id : _strings_64)
		{
			ConfigValue& entry = root["string_id_64"].Append();
			entry.SetEmptyArray();
			entry.Append().SetUInt(id.first);
			entry.Append().SetString(id.second.c_str());
		}

		FileStreamPtr file = _file_source->OpenFile(_target_path.c_str(), File::WRITE);
		if (!file.Get())
		{
			logging::Warning("Failed to save StringId repository.");
			return;
		}

		json::Writer writer;
		stringstream ss;
		writer.Write(root, ss, true);

		file->Write(ss.str().c_str(), (uint32_t)ss.str().size());

		logging::Info("StringId repository successfully saved.");
	}
	bool StringIdRepository::Load()
	{
		Assert(_file_source);
		Assert(!_target_path.empty());

		ConfigValue root;

		json::Reader reader;
		if (!reader.ReadFile(_file_source, _target_path.c_str(), root))
		{
			logging::Warning("Failed loading StringId repository: %s", reader.GetErrorMessage().c_str());
			return false;
		}

		const ConfigValue& string_id_32 = root["string_id_32"];
		if (string_id_32.IsArray())
		{
			for (uint32_t i = 0; i < string_id_32.Size(); ++i)
			{
				if (string_id_32[i].IsArray() && string_id_32[i].Size() == 2)
				{
					if (!string_id_32[i][0].IsNumber() || !string_id_32[i][1].IsString())
						continue;

					Add(string_id_32[i][0].AsUInt(), string_id_32[i][1].AsString(), (uint32_t)strlen(string_id_32[i][1].AsString()));
				}
			}
		}

		const ConfigValue& string_id_64 = root["string_id_64"];
		if (string_id_64.IsArray())
		{
			for (uint32_t i = 0; i < string_id_64.Size(); ++i)
			{
				if (string_id_64[i].IsArray() && string_id_64[i].Size() == 2)
				{
					if (!string_id_64[i][0].IsNumber() || !string_id_64[i][1].IsString())
						continue;

					Add(string_id_64[i][0].AsUInt64(), string_id_64[i][1].AsString(), (uint32_t)strlen(string_id_64[i][1].AsString()));
				}
			}
		}


		logging::Info("StringId repository successfully loaded.");
		return true;
	}

} // namespace sb
