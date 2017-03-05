// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ShaderCompiler.h"
#include "ShaderDatabase.h"
#include "DependencyDatabase.h"
#include "D3D11/D3D11Platform.h"

#include <Foundation/Json/Json.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/IO/FileInputBuffer.h>

#include <Engine/Rendering/Shader.h>
#include <Engine/Rendering/ShaderLibrary.h>

namespace sb
{

	ShaderCompiler::ShaderCompiler(const ConfigValue& config)
		: CompilerSystem::Compiler(config)
	{
	}
	ShaderCompiler::~ShaderCompiler()
	{
	}

	CompilerSystem::Result ShaderCompiler::Compile(const FilePath& source_file, const FilePath& target_file, const CompilerSystem::CompilerContext& context)
	{
		FilePath shader_name = source_file;
		shader_name.TrimExtension();
		shader_name.SetSeparator('/');

		ConfigValue shader_cfg;

		simplified_json::Reader reader;
		if (!reader.ReadFile(context.asset_source, source_file.c_str(), shader_cfg))
		{
			SetError(reader.GetErrorMessage().c_str());
			return CompilerSystem::FAILED;
		}

		if (!context.shader_database->HasShader(shader_name.c_str()))
		{
			context.shader_database->InsertShader(source_file, shader_cfg);
		}

		ShaderDatabase::Shader& shader = context.shader_database->GetShader(shader_name.c_str());
		ShaderDatabase::Shader::PermutationMap& permutations = shader.GetPermutations();
		shader.Update(shader_cfg);
		shader.SetDirty(false);

		D3D11Platform platform(source_file.Directory(), context);
		platform.GetCache().RecordDependencies(source_file, context.dependency_database);

		platform.Precompile(shader);

		ShaderLibrary shader_library;
		shader_library.permutation_count = (uint32_t)permutations.size();
		shader_library.permutations = new ShaderData[permutations.size()];
		shader_library.render_resources = NULL;

		uint32_t i = 0;
		for (auto& permutation : permutations)
		{
			shader_library.permutations[i] = ShaderData();

			if (!CompilePermutation(shader, permutation.second.options, context, platform, shader_library.permutations[i]))
				return CompilerSystem::FAILED;

			++i;
		}

		CompilerSystem::Result res = CompilerSystem::SUCCESSFUL;
		if (shader_library.permutation_count)
		{
			vector<uint8_t> shader_target_data;
			DynamicMemoryStream stream(&shader_target_data);
			shader_library_resource::Compile(shader_library, stream);

			if (!WriteAsset(context.asset_target, target_file, shader_target_data.data(), (uint32_t)shader_target_data.size()))
			{
				res = CompilerSystem::FAILED;
			}
		}

		// Cleanup
		for (uint32_t p = 0; p < shader_library.permutation_count; ++p)
		{
			platform.Cleanup(shader_library.permutations[p]);
		}

		delete[] shader_library.permutations;

		return res;
	}

	bool ShaderCompiler::CompilePermutation(const ShaderDatabase::Shader& shader, const vector<string>& defines,
		const CompilerSystem::CompilerContext&, D3D11Platform& platform,
		ShaderData& out_shader_data)
	{
		string permutation_name;
		FilePath permutation_path;

		shader.BuildName(defines, permutation_name);
		out_shader_data.name = permutation_name;

		if (!platform.Compile(shader, defines, out_shader_data))
		{
			platform.Cleanup(out_shader_data);
			SetError(platform.GetError().c_str());
			return false;
		}

		logging::Info("Shader permutation '%s' compiled", permutation_name.c_str());

		return true;
	}

	//-------------------------------------------------------------------------------

	ShaderCompiler::ShaderFileCache::ShaderFileCache(FileSource* file_source)
		: _file_source(file_source),
		_dependency_database(NULL)
	{
	}
	ShaderCompiler::ShaderFileCache::~ShaderFileCache()
	{
		Clear();
	}

	void ShaderCompiler::ShaderFileCache::RecordDependencies(const FilePath& resource, DependencyDatabase* database)
	{
		_dependent_path = resource;
		_dependency_database = database;
	}

	InputBuffer* ShaderCompiler::ShaderFileCache::Open(const char* file_path)
	{
		// Look if file is already loaded
		map<StringId64, InputBuffer*>::iterator it = _files.find(file_path);
		if (it != _files.end())
		{
			return it->second;
		}

		FileStreamPtr file = _file_source->OpenFile(file_path, File::READ);
		if (!file.Get())
			return NULL;

		if (_dependency_database)
			_dependency_database->AddDependent(file_path, _dependent_path.c_str());

		InputBuffer* buffer = new FileInputBuffer(file);

		_files[file_path] = buffer;
		return buffer;
	}
	void ShaderCompiler::ShaderFileCache::Clear()
	{
		for (auto& file : _files)
		{
			delete file.second;
		}
		_files.clear();
	}

	//-------------------------------------------------------------------------------


} // namespace sb

