// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "CompilerSystem.h"
#include "BuildServer.h"
#include "DependencyDatabase.h"
#include "ShaderDatabase.h"
#include "BuildUICallback.h"

#include <Foundation/Thread/TaskScheduler.h>

namespace sb
{

	//-------------------------------------------------------------------------------
	CompilerSystem::Compiler::Compiler(const ConfigValue& config)
	{
		Assert(config["type"].IsString());
		Assert(config["source_type"].IsString());

		_type = config["type"].AsString();
		_source_type = config["source_type"].AsString();

	}

	void CompilerSystem::Compiler::SetError(const char* msg)
	{
		_last_error = msg;
	}
	bool CompilerSystem::Compiler::WriteAsset(FileSource* asset_target, const FilePath& path, const uint8_t* data, uint32_t len)
	{
		// Make sure folder exists
		asset_target->MakeDirectory(path.Directory().c_str());

		FileStreamPtr file = asset_target->OpenFile(path.c_str(), File::WRITE);
		if (!file.Get())
		{
			logging::Warning("Failed to write to target file '%s'", path.c_str());
			return false;
		}

		file->Write(data, len);
		return true;
	}
	bool CompilerSystem::Compiler::NeedCompile(const FilePath& source_file, const FilePath& target_file, const CompilerContext& context)
	{
		FileTime source_time, target_time;

		if (!context.asset_source->LastModifiedTime(source_file.c_str(), source_time))
		{
			// Source file not found?
			logging::Warning("Failed to retrieve last modified time for source file '%s'", source_file.c_str());
			return false;
		}

		if (!context.asset_target->LastModifiedTime(target_file.c_str(), target_time)
			|| (target_time < source_time))
		{
			return true;
		}
		return false;
	}

	const string& CompilerSystem::Compiler::GetType() const
	{
		return _type;
	}
	const string& CompilerSystem::Compiler::GetSourceType() const
	{
		return _source_type;
	}
	const string& CompilerSystem::Compiler::GetLastError() const
	{
		return _last_error;
	}

	//-------------------------------------------------------------------------------

	CompilerSystem::CompilerSystem(BuildServer* builder, FileSource* asset_source, FileSource* asset_target,
		DependencyDatabase* dependency_db, ShaderDatabase* shader_db, BuildSettings* settings)
		: _builder(builder),
		_asset_source(asset_source),
		_asset_target(asset_target),
		_shader_database(shader_db),
		_dependency_database(dependency_db),
		_active_settings(settings)
	{
	}
	CompilerSystem::~CompilerSystem()
	{

	}
	void CompilerSystem::Compile(AssetSource* sources, uint32_t num, bool force, BuildUICallback* callback)
	{
		Assert(sources);
		Assert(_builder);

		CompilerContext context(_asset_source, _asset_target, _dependency_database,
			_shader_database, _active_settings);

		FilePath target_path;
		vector<string> dependents;


		if (num > 1 && callback)
		{
			callback->OnCompileBatch(sources, num);
		}

		for (uint32_t i = 0; i < num; ++i)
		{
			if (_builder->IsStopping())
				break;
			if (sources[i].source_type == "") // Skip sources without types (Files without extensions)
				continue;

			// Skip ignored assets
			if (std::find(_ignores.begin(), _ignores.end(), sources[i].source_path.c_str()) != _ignores.end())
				continue;

			Compiler* compiler;

			map<StringId32, Compiler*>::iterator it = _compilers.find(sources[i].source_type);
			if (it == _compilers.end())
			{
				// File is an external resource, check with dependency database if it has any last modified time
				//	to check if it has changed since last time.
				FileTime old_file_time;
				FileTime new_file_time;

				bool need_compile = true;
				if (context.asset_source->LastModifiedTime(sources[i].source_path.c_str(), new_file_time)
					&& _dependency_database->GetModifiedTime(sources[i].source_path.c_str(), old_file_time))
				{
					if (new_file_time <= old_file_time)
						need_compile = false;
				}

				if (need_compile)
				{
					// Files without compilers may still have dependents so check them now

					_dependency_database->GetDependents(sources[i].source_path.c_str(), dependents);
					if (dependents.size() > 0)
					{
						logging::Info("Compiling %d dependencies for %s", dependents.size(), sources[i].source_path.c_str());
					}
					for (vector<string>::iterator it = dependents.begin(); it != dependents.end(); ++it)
					{
						AssetSource dependent_source(it->c_str());
						Compile(&dependent_source, 1, true);
					}
					if (dependents.size())
					{
						// Only update time for resources with dependencies
						_dependency_database->UpdateModifiedTime(sources[i].source_path.c_str(), new_file_time);
					}

					dependents.clear();
				}


				if (callback)
				{
					callback->OnCompileSkip(sources[i].source_path, target_path);
				}
				continue;
			}
			compiler = it->second;
			Assert(compiler);

			target_path += sources[i].source_path.c_str();
			target_path.TrimExtension();
			target_path += ".";
			target_path += compiler->GetType();

			if (force || compiler->NeedCompile(sources[i].source_path, target_path, context))
			{
				// Needs recompile
				if (callback)
				{
					callback->OnCompile(sources[i].source_path, target_path);
				}

				Result res = compiler->Compile(sources[i].source_path, target_path, context);
				if (res == FAILED)
				{
					logging::Warning("Failed to compile '%s': %s", sources[i].source_path.c_str(), compiler->GetLastError().c_str());
					if (callback)
					{
						callback->OnCompileFailed(sources[i].source_path, target_path, compiler->GetLastError());
					}
				}
				else if (res == SUCCESSFUL)
				{
					logging::Info("File successfuly compiled: %s (Type: %s)", sources[i].source_path.c_str(), sources[i].source_type.c_str());
					if (callback)
					{
						callback->OnCompileSuccessful(sources[i].source_path, target_path);
					}

					// Compile dependencies
					_dependency_database->GetDependents(sources[i].source_path.c_str(), dependents);
					if (dependents.size() > 0)
					{
						logging::Info("Compiling %d dependencies for %s", dependents.size(), sources[i].source_path.c_str());
					}
					for (vector<string>::iterator it = dependents.begin(); it != dependents.end(); ++it)
					{
						AssetSource dependent_source(it->c_str());
						Compile(&dependent_source, 1, true);
					}
					dependents.clear();
				}

			}
			else
			{
				if (callback)
				{
					callback->OnCompileSkip(sources[i].source_path, target_path);
				}
			}

			target_path.Clear();
		}
	}

	void CompilerSystem::RegisterCompiler(const char* source_type, Compiler* compiler)
	{
		_compilers[source_type] = compiler;
	}
	void CompilerSystem::UnregisterCompiler(Compiler* compiler)
	{
		_compilers.erase(compiler->GetSourceType());
	}

	void CompilerSystem::SetBuildSettings(BuildSettings* settings)
	{
		_active_settings = settings;
	}

	void CompilerSystem::AddIgnoreAsset(const char* asset_name)
	{
		_ignores.push_back(asset_name);
	}

} // namespace sb

