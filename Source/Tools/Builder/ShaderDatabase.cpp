// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ShaderDatabase.h"

#include <Foundation/Json/Json.h>

namespace sb
{

	//-------------------------------------------------------------------------------
	ShaderDatabase::Shader::Shader(const FilePath& source_path, const ConfigValue& shader_cfg)
		: _source(source_path.c_str()),
		_dirty(false)
	{

		FilePath path(source_path);
		path.TrimExtension();
		_name = path.Get();

		_shader_cfg = shader_cfg;
		Assert(_shader_cfg.IsObject());

		// Always preload a shader with 0 options that we can use as default
		FilePath name = _name;
		name.TrimExtension();

		_permutations.insert(pair<string, Permutation>(name.Get(), Permutation()));
	}
	ShaderDatabase::Shader::~Shader()
	{
	}
	bool ShaderDatabase::Shader::PreloadPermutation(const vector<string>& options)
	{
		if (options.empty())
		{
			// There always exist a permutation for 0 options
			return true;
		}

		string permutation_name;
		BuildName(options, permutation_name);

		PermutationMap::iterator map_it = _permutations.find(permutation_name);
		if (map_it != _permutations.end())
		{
			// Requested permutation already exists
			return true;
		}

		ConfigValue& options_node = _shader_cfg["options"];
		if (!options_node.IsArray())
		{
			// No options available
			return false;
		}

		if (!IsValidOptions(options))
			return false;

		_permutations.insert(pair<string, Permutation>(permutation_name, Permutation(options)));
		_dirty = true;

		return true;
	}

	ShaderDatabase::Shader::PermutationMap& ShaderDatabase::Shader::GetPermutations()
	{
		return _permutations;
	}
	void ShaderDatabase::Shader::BuildName(const vector<string>& options, string& name) const
	{
		FilePath tmp = _name;
		tmp.TrimExtension();
		name = tmp.Get();

		if (options.empty())
			return;


		// The given options can be in a different order compared to m_options, therefore
		//	we need to make sure they get translated in the right order.


		const ConfigValue& options_node = _shader_cfg["options"];
		if (!options_node.IsArray())
		{
			// No options available
			return;
		}

		for (uint32_t i = 0; i < options_node.Size(); ++i)
		{
			if (!options_node[i].IsObject() || !options_node[i]["define"].IsString())
			{
				logging::Warning("Invalid options for shader '%s'.", _name.c_str());
				continue;
			}

			for (vector<string>::const_iterator cit = options.begin(); cit != options.end(); ++cit)
			{
				if (*cit == options_node[i]["define"].AsString())
				{
					name += ":";
					name += options_node[i]["define"].AsString();
					break;
				}
			}
		}

	}
	bool ShaderDatabase::Shader::IsValidOptions(const vector<string>& options) const
	{
		// The given options can be in a different order compared to m_options, therefore
		//	we need to make sure they get translated in the right order.

		const ConfigValue& options_node = _shader_cfg["options"];
		if (!options_node.IsArray() || options_node.Size() == 0)
		{
			if (options.size() == 0)
				return true;
			else
				return false;
		}

		vector<string>::const_iterator cit, cend;
		cit = options.begin(); cend = options.end();
		for (; cit != cend; ++cit)
		{
			bool found = false;
			for (uint32_t i = 0; i < options_node.Size(); ++i)
			{
				Assert(options_node[i].IsObject());
				Assert(options_node[i]["define"].IsString());

				if (*cit == options_node[i]["define"].AsString())
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				// Invalid shader options
				return false;
			}
		}

		return true;
	}
	void ShaderDatabase::Shader::Update(const ConfigValue& shader_cfg)
	{
		_shader_cfg = shader_cfg;
		Assert(_shader_cfg.IsObject());

		// Validate all permutations as the options may have changed
		PermutationMap::iterator it, end;
		it = _permutations.begin(); end = _permutations.end();
		while (it != end)
		{
			if (!IsValidOptions(it->second.options))
			{
				logging::Warning("Invalid permutation in shader %s", _name.c_str());
				_permutations.erase(it);
			}
			else
				++it;
		}
	}

	const string& ShaderDatabase::Shader::GetName() const
	{
		return _name;
	}
	const AssetSource& ShaderDatabase::Shader::GetSource() const
	{
		return _source;
	}
	const ConfigValue& ShaderDatabase::Shader::GetConfig() const
	{
		return _shader_cfg;
	}
	bool ShaderDatabase::Shader::IsDirty() const
	{
		return _dirty;
	}
	void ShaderDatabase::Shader::SetDirty(bool bDirty)
	{
		_dirty = bDirty;
	}

	//-------------------------------------------------------------------------------
	ShaderDatabase::ShaderDatabase(FileSource* file_source)
		: _file_source(file_source)
	{
	}
	ShaderDatabase::~ShaderDatabase()
	{
	}
	void ShaderDatabase::Save()
	{
		Assert(_file_source);

		ConfigValue root;
		root.SetEmptyObject();


		map<StringId64, Shader>::iterator map_it, map_end;
		map_it = _shaders.begin(); map_end = _shaders.end();
		for (; map_it != map_end; ++map_it)
		{
			ConfigValue& entry = root[map_it->second.GetName().c_str()];
			entry.SetEmptyObject();

			ConfigValue& permutations = entry["permutations"];
			permutations.SetEmptyArray();

			Shader::PermutationMap::const_iterator it, end;
			it = map_it->second.GetPermutations().begin(); end = map_it->second.GetPermutations().end();
			for (; it != end; ++it)
			{
				ConfigValue& permutation = permutations.Append();
				permutation.SetEmptyArray();
				for (vector<string>::const_iterator per_it = it->second.options.begin(); per_it != it->second.options.end(); ++per_it)
				{
					permutation.Append().SetString(per_it->c_str());
				}

			}
		}

		// Make sure .builder folder exists
		_file_source->MakeDirectory(".builder");

		FileStreamPtr file = _file_source->OpenFile(".builder/shader_database", File::WRITE);
		if (!file.Get())
		{
			logging::Warning("Failed to save shader database");
			return;
		}

		json::Writer writer;
		stringstream ss;
		writer.Write(root, ss, true);

		file->Write(ss.str().c_str(), ss.str().size());

		logging::Info("Shader database successfully saved.");
	}
	bool ShaderDatabase::Load()
	{
		Assert(_file_source);
		ConfigValue root;

		json::Reader reader;
		if (!reader.ReadFile(_file_source, ".builder/shader_database", root))
		{
			logging::Warning("Failed loading shader database: %s", reader.GetErrorMessage().c_str());
			return false;
		}

		FilePath shader_path;
		vector<string> options;


		ConfigValue::Iterator it, end;
		it = root.Begin(); end = root.End();
		for (; it != end; ++it)
		{
			StringId64 shader_id(it->first);
			ConfigValue& entry = it->second;

			Assert(entry.IsObject());
			if (!entry.IsObject())
				continue;

			shader_path.Set(it->first);
			shader_path += ".shader_src";

			ConfigValue shader_cfg;
			if (!reader.ReadFile(_file_source, shader_path.c_str(), shader_cfg))
			{
				logging::Warning("Failed loading shader '%s': %s", shader_path.c_str(), reader.GetErrorMessage().c_str());
				continue;
			}

			_shaders.insert(pair<StringId64, Shader>(shader_id,
				Shader(shader_path, shader_cfg))).first;
			options.clear();

			Shader& shader = _shaders.find(shader_id)->second;

			ConfigValue& permutation_node = entry["permutations"];
			for (uint32_t i = 0; i < permutation_node.Size(); ++i)
			{
				Assert(permutation_node[i].IsArray());
				if (!permutation_node[i].IsArray())
					continue;
				for (uint32_t j = 0; j < permutation_node[i].Size(); ++j)
				{
					Assert(permutation_node[i][j].IsString());
					if (!permutation_node[i][j].IsString())
						continue;

					options.push_back(string(permutation_node[i][j].AsString()));
				}

				shader.PreloadPermutation(options);
				options.clear();
			}

			shader.SetDirty(false);
			shader_path.Clear();
		}

		logging::Info("Shader database successfully loaded.");
		return true;
	}
	void ShaderDatabase::Clear()
	{
		_shaders.clear();
	}
	bool ShaderDatabase::PreloadShader(const char* shader_name)
	{
		map<StringId64, Shader>::iterator shader_it = _shaders.find(StringId64(shader_name));
		if (shader_it != _shaders.end())
		{
			// Shader already loaded
			return true;
		}

		ConfigValue root;
		FilePath shader_path = shader_name;
		shader_path += ".shader_src";

		simplified_json::Reader reader;
		if (!reader.ReadFile(_file_source, shader_path.c_str(), root))
		{
			logging::Warning("Failed to preload shader: %s", reader.GetErrorMessage().c_str());
			return false;
		}

		AssetSource shader_src(shader_path.c_str());

		Assert(root.IsObject());
		if (!root.IsObject())
			return false;


		InsertShader(shader_src.source_path, root);

		logging::Info("Shader '%s' successfully preloaded", shader_name);

		return true;
	}
	bool ShaderDatabase::PreloadPermutation(const char* shader_name, const vector<string>& per_options)
	{
		map<StringId64, Shader>::iterator shader_it = _shaders.find(StringId64(shader_name));
		if (shader_it != _shaders.end())
		{
			if (!shader_it->second.PreloadPermutation(per_options))
			{
				logging::Warning("Failed to preload shader permutation: Invalid shader options");
				return false;
			}
			return true;
		}
		// Shader not found so we try to preload it first
		if (!PreloadShader(shader_name))
		{
			// Preload failed
			return false;
		}

		shader_it = _shaders.find(StringId64(shader_name));
		if (shader_it == _shaders.end())
			return false;

		if (!shader_it->second.PreloadPermutation(per_options))
		{
			logging::Warning("Failed to preload shader permutation: Invalid shader options");
			return false;
		}

		string permutation_name;
		shader_it->second.BuildName(per_options, permutation_name);

		logging::Info("Shader permutation '%s' preloaded", permutation_name.c_str());

		return true;

	}
	bool ShaderDatabase::HasShader(const char* shader_name) const
	{
		map<StringId64, Shader>::const_iterator it = _shaders.find(StringId64(shader_name));
		if (it != _shaders.end())
			return true;

		return false;
	}
	ShaderDatabase::Shader& ShaderDatabase::GetShader(const char* shader_name)
	{
		map<StringId64, Shader>::iterator it = _shaders.find(StringId64(shader_name));
		if (it == _shaders.end())
		{
			Assert(false);
			logging::Error("ShaderDatabase::GetShader() : Shader was not found");
		}

		return it->second;
	}

	bool ShaderDatabase::IsDirty() const
	{
		for (auto& shader : _shaders)
		{
			if (shader.second.IsDirty())
				return true;
		}
		return false;
	}
	void ShaderDatabase::GetDirtyShaders(vector<AssetSource>& sources) const
	{
		for (auto& shader : _shaders)
		{
			if (shader.second.IsDirty())
				sources.push_back(shader.second.GetSource());
		}
	}

	//-------------------------------------------------------------------------------
	void ShaderDatabase::InsertShader(const FilePath& source_path, const ConfigValue& shader_cfg)
	{
		FilePath shader_name = source_path;
		shader_name.TrimExtension();
		shader_name.SetSeparator('/');

		map<StringId64, Shader>::iterator it = _shaders.find(StringId64(shader_name.c_str()));
		if (it != _shaders.end())
		{
			// Shader already in database, update it
			it->second.Update(shader_cfg);
		}
		else
		{
			_shaders.insert(pair<StringId64, Shader>(StringId64(shader_name.c_str()), Shader(source_path, shader_cfg))).first;
		}
	}
	//-------------------------------------------------------------------------------

} // namespace sb

