// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "BuildServer.h"
#include "ShaderDatabase.h"
#include "DependencyDatabase.h"
#include "BuildUICallback.h"

#include <Foundation/Debug/Console.h>
#include <Foundation/Debug/ConsoleServer.h>
#include <Foundation/Filesystem/DirectoryWatcher.h>
#include <Foundation/Filesystem/FilePath.h>
#include <Foundation/Filesystem/FileUtil.h>
#include <Foundation/Json/Json.h>
#include <Foundation/Container/StringIdRepository.h>


#include "MaterialCompiler.h"
#include "ShaderCompiler.h"
#include "TextureCompiler.h"
#include "MeshCompiler.h"
#include "ScriptCompiler.h"
#include "PackageCompiler.h"
#include "CopyCompiler.h"
#include "FontCompiler.h"

namespace sb
{

	namespace
	{
		static const char* level_to_string[] = {
			/* LOG_INFO		*/	"info",
			/* LOG_WARNING	*/	"warning",
			/* LOG_ERROR	*/	"error"
		};

		void LoggingCallback(void* data, Log::LogLevel level, const char* msg)
		{
			ConsoleServer* console = (ConsoleServer*)data;
			if (console)
			{
				ConfigValue msg_data;
				msg_data.SetEmptyObject();
				msg_data["type"].SetString("log_msg");
				msg_data["level"].SetString(level_to_string[level]);
				msg_data["msg"].SetString(msg);

				console->Send(msg_data);
			}

			printf("%s\n", msg);
		}
	}

	namespace build_server_commands
	{
		void FullRebuild(void* user_data, const ConfigValue&)
		{
			BuildServer* server = (BuildServer*)user_data;
			server->FullRebuild();
		}

		void StringIdLookup(void* user_data, const ConfigValue& cmd)
		{
			if (cmd["args"].Size() == 0 || !cmd["args"][0].IsString())
			{
				logging::Info("Usage: string_id_lookup <string_id>");
				logging::Info("Example: string_id_lookup 0xAABBCCDD");
				return;
			}

			const char* hex_str = cmd["args"][0].AsString();
			uint32_t str_len = (uint32_t)strlen(hex_str);

			if (str_len < 2 || hex_str[0] != '0' || (hex_str[1] != 'x' && hex_str[1] != 'X'))
			{
				logging::Info("Wrong format of string id, expecting:");
				logging::Info("\t0xAABBCCDD\t\t: For 32bit string identifiers");
				logging::Info("\t0x11223344AABBCCDD\t: For 64bit string identifiers");
				return;
			}

			StringIdRepository* repo = (StringIdRepository*)user_data;
			Assert(repo);

			const char* result;
			if (str_len <= 10)
			{
				uint32_t id;
				id = strtoul(hex_str, NULL, 0);
				result = repo->LookUp(id);
			}
			else
			{
				uint64_t id;
				id = _strtoui64(hex_str, NULL, 0);
				result = repo->LookUp(id);
			}

			if (result)
				logging::Info("%s => \"%s\"", hex_str, result);
			else
				logging::Info("No match found.");
		}
	};

	BuildServer::BuildServer(const BuilderParams& params)
		: _file_system(params.base_path.c_str()),
		_params(params),
		_stop(false),
		_dependency_database(NULL),
		_shader_database(NULL),
		_callback(NULL),
		_active_settings(NULL)
	{
		logging::SetCallback(LoggingCallback, nullptr);

		logging::Info("*** Running Builder ***");

		_source = _file_system.OpenFileSource(params.source_path.c_str());
		_target = _file_system.OpenFileSource(params.target_path.c_str());
		// Make sure target directory exists
		_file_system.MakeDirectory(params.target_path.c_str());

		// Make sure .builder folder exists
		_source->MakeDirectory(".builder");

		_dependency_database = new DependencyDatabase(_source);
		_shader_database = new ShaderDatabase(_source);
		_string_id_repository = new StringIdRepository(_source, ".builder/string_id_repository");
		_string_id_repository->Load();

		string_id::SetRepository(_string_id_repository);

		if (_params.server) // No need to watch directories if we're not running in server mode
		{

			_dir_watcher = new DirectoryWatcher(_source->GetFullPath().c_str(), true);
		}

		ConfigValue settings_cfg;

		simplified_json::Reader reader;
		if (!reader.ReadFile(_source, "builder.settings", settings_cfg))
		{
			logging::Warning("Failed to load builder settings: %s", reader.GetErrorMessage().c_str());
			return;
		}

		if (_params.server && (settings_cfg["console_server_port"].IsNumber())) // No need to start the console server if we're not running in server mode
		{
			console::Initialize(settings_cfg["console_server_port"].AsInt());
			logging::SetCallback(LoggingCallback, (void*)console::Server());
		}

		if (settings_cfg["setting_profiles"].IsObject())
		{
			ConfigValueConstIterator	prof_it = settings_cfg["setting_profiles"].Begin(),
				prof_end = settings_cfg["setting_profiles"].End();
			for (; prof_it != prof_end; ++prof_it)
			{
				BuildSettings* settings = new BuildSettings();
				build_settings::Load(prof_it->second, *settings);
				_setting_profiles[prof_it->first] = settings;

				if (!_active_settings)
					_active_settings = settings;
			}
		}

		if (_setting_profiles.empty())
		{
			logging::Warning("No setting profiles defined, setting to default.");
			_active_settings = new BuildSettings();
			_setting_profiles["default"] = _active_settings;

		}

		_compiler_system = new CompilerSystem(this, _source, _target, _dependency_database, _shader_database, _active_settings);

		RegisterCompilers(settings_cfg["compilers"]);

		// Parse ignore list
		if (settings_cfg["ignore_list"].IsArray())
		{
			for (uint32_t i = 0; i < settings_cfg["ignore_list"].Size(); ++i)
			{
				if (settings_cfg["ignore_list"][i].IsString())
					_compiler_system->AddIgnoreAsset(settings_cfg["ignore_list"][i].AsString());
			}
		}

		ADD_CONSOLE_COMMAND("full_rebuild", build_server_commands::FullRebuild, this, "Rebuilds all assets.");
		ADD_CONSOLE_COMMAND("string_id_lookup", build_server_commands::StringIdLookup, _string_id_repository, "Looks up a string from a string identifier.");
	}
	BuildServer::~BuildServer()
	{
		for (CompilerSystem::Compiler* compiler : _compilers)
		{
			delete compiler;
		}
		_compilers.clear();

		_active_settings = NULL;
		if (!_setting_profiles.empty())
		{
			for (auto& settings : _setting_profiles)
			{
				delete settings.second;
			}
			_setting_profiles.clear();
		}

		if (_params.server)
		{
			delete _dir_watcher;
		}


		_shader_database->Save();
		_dependency_database->Save();
		_string_id_repository->Save();
		string_id::SetRepository(NULL);

		delete _compiler_system;
		delete _shader_database;
		delete _dependency_database;
		delete _string_id_repository;

		_file_system.CloseFileSource(_source);
		_file_system.CloseFileSource(_target);

		if (_params.server && console::Initialized())
		{
			logging::SetCallback(NULL, NULL);
			console::Shutdown();
		}
	}
	void BuildServer::SetCallback(BuildUICallback* callback)
	{
		_callback = callback;
	}
	void BuildServer::Run()
	{
		bool force_compile = false;
		if (!_dependency_database->Load() || !_shader_database->Load()) // Force a complete compile if the dependency/shader database failed to load
		{
			logging::Info("Missing shader or dependency database: Forcing a full compile");
			force_compile = true;
		}

		if (force_compile || _params.force_recompile)
		{
			FullRebuild();
		}
		else
		{
			PartialRebuild();
		}

		if (!_params.server)
		{
			Stop();
			return;
		}


		vector<Action> actions;
		vector<AssetSource> changes;
		vector<DirectoryChange> pending_changes;

		DirectoryChange dir_change;
		while (!_stop)
		{
			// Handle any directory changes
			while (_dir_watcher->PopChange(dir_change))
			{
				FilePath path(dir_change.path);
				path.SetSeparator('/');
				if (path.Directory() == ".builder/") // Skip .builder as builder uses that directory internally
					continue;

				// Windows always sends two FILE_MODIFIED notifications when a file is modified,
				//	to make sure we only dispatch the asset to the compiler when the file is actually
				//	ready we let the first notifaction pend until the second one arrives.

				if (dir_change.action != DirectoryChange::FILE_MODIFIED)
					changes.push_back(AssetSource(path.c_str()));

				// Filter any duplicated notifications as we always get two when file is modified
				bool found = false;
				for (vector<DirectoryChange>::iterator it = pending_changes.begin(); it != pending_changes.end(); ++it)
				{
					if (it->path == dir_change.path) // this is the second notifaction
					{
						found = true;
						changes.push_back(AssetSource(path.c_str()));
						pending_changes.erase(it);
						break;
					}
				}
				if (!found)
					pending_changes.push_back(dir_change);
			}
			if (!changes.empty())
			{
				// If we dispatch the changed file to the compiler to quickly it won't be able to open the source file 
				//		as the file is already opened by another program.
				Sleep(100); // TODO: Find better solution than this maybe?
				_compiler_system->Compile(changes.data(), (uint32_t)changes.size(), false, _callback);
				changes.clear();
			}
			if (_shader_database->IsDirty())
			{
				_shader_database->GetDirtyShaders(changes);
				if (!changes.empty())
				{
					_compiler_system->Compile(changes.data(), (uint32_t)changes.size(), true, _callback);
					changes.clear();
				}
			}



			{
				ScopedLock<CriticalSection> lock(_action_queue_lock);
				if (!_action_queue.empty())
				{
					actions.push_back(_action_queue.front());
					_action_queue.pop();
				}
			}
			if (!actions.empty())
			{
				for (vector<Action>::iterator it = actions.begin(); it != actions.end(); ++it)
				{
					switch (*it)
					{
					case SCAN:
						PartialRebuild();
						break;
					case FULL_REBUILD:
						FullRebuild();
						break;
					case CHANGE_PROFILE:
						if (!_queued_profile.empty())
						{
							_active_settings = _setting_profiles[_queued_profile];
							_queued_profile = "";
							_compiler_system->SetBuildSettings(_active_settings);
							// Trigger full rebuild when changing profile
							FullRebuild();
						}
						break;
					};
				}
			}
			actions.clear();

			console::Server()->Update();
			Sleep(1);
		}
	}
	void BuildServer::Stop()
	{
		_stop = true;
	}
	bool BuildServer::IsStopping() const
	{
		return _stop;
	}

	void BuildServer::QueueFullRebuild()
	{
		ScopedLock<CriticalSection> lock(_action_queue_lock);
		_action_queue.push(FULL_REBUILD);
	}
	void BuildServer::QueueScan()
	{
		ScopedLock<CriticalSection> lock(_action_queue_lock);
		_action_queue.push(SCAN);
	}

	void BuildServer::SetActiveProfile(const string& profile)
	{
		_queued_profile = profile;
		ScopedLock<CriticalSection> lock(_action_queue_lock);
		_action_queue.push(CHANGE_PROFILE);
	}

	void BuildServer::FullRebuild()
	{
		// First we clear the dependency database because we will force all assets
		//	to recompile and therefore we get a new complete database, this will also
		//	clean out any redundant dependencies that may exist
		_dependency_database->Clear();
		_shader_database->Clear();

		vector<AssetSource> assets;
		ScanSourceDirectory(assets);

		if (!assets.empty())
		{
			_compiler_system->Compile(assets.data(), (uint32_t)assets.size(), true, _callback);
		}

		_dependency_database->Save();
		_shader_database->Save();
		_string_id_repository->Save();
	}
	void BuildServer::PartialRebuild()
	{
		vector<AssetSource> assets;
		ScanSourceDirectory(assets);

		if (!assets.empty())
		{
			_compiler_system->Compile(assets.data(), (uint32_t)assets.size(), false, _callback);
		}

	}

	void BuildServer::ScanSourceDirectory(vector<AssetSource>& sources)
	{
		string find_path;
		vector<string> files;

		file_util::FindFilesRecursive(_source, "*", files);

		for (vector<string>::iterator it = files.begin(); it != files.end(); ++it)
		{
			if (it->substr(0, 8) == ".builder") // Ignore .builder folder
				continue;

			sources.push_back(AssetSource(it->c_str()));
		}

	}
	void BuildServer::RegisterCompilers(const ConfigValue& compilers)
	{
		Assert(compilers.IsArray());

		string action;
		for (uint32_t i = 0; i < compilers.Size(); ++i)
		{
			Assert(compilers[i].IsObject());
			Assert(compilers[i]["action"].IsString());
			Assert(compilers[i]["source_type"].IsString());

			action = compilers[i]["action"].AsString();
			const char* source_type = compilers[i]["source_type"].AsString();

			CompilerSystem::Compiler* compiler;
			if (action == "material_compiler")
			{
				compiler = new MaterialCompiler(compilers[i]);

				_compiler_system->RegisterCompiler(source_type, compiler);
				_compilers.push_back(compiler);
			}
			else if (action == "shader_compiler")
			{
				compiler = new ShaderCompiler(compilers[i]);

				_compiler_system->RegisterCompiler(source_type, compiler);
				_compilers.push_back(compiler);
			}
			else if (action == "texture_compiler")
			{
				compiler = new TextureCompiler(compilers[i]);

				_compiler_system->RegisterCompiler(source_type, compiler);
				_compilers.push_back(compiler);
			}
			else if (action == "mesh_compiler")
			{
				compiler = new MeshCompiler(compilers[i]);

				_compiler_system->RegisterCompiler(source_type, compiler);
				_compilers.push_back(compiler);
			}
			else if (action == "script_compiler")
			{
				compiler = new ScriptCompiler(compilers[i]);

				_compiler_system->RegisterCompiler(source_type, compiler);
				_compilers.push_back(compiler);
			}
			else if (action == "package_compiler")
			{
				compiler = new PackageCompiler(compilers[i]);
				_compiler_system->RegisterCompiler(source_type, compiler);
				_compilers.push_back(compiler);
			}
			else if (action == "copy")
			{
				compiler = new CopyCompiler(compilers[i]);
				_compiler_system->RegisterCompiler(source_type, compiler);
				_compilers.push_back(compiler);
			}
			else if (action == "font_compiler")
			{
				compiler = new FontCompiler(compilers[i]);
				_compiler_system->RegisterCompiler(source_type, compiler);
				_compilers.push_back(compiler);
			}
			else
			{
				logging::Warning("Build action '%s' not found", action.c_str());
				continue;
			}

		}

	}

	const map<string, BuildSettings*>& BuildServer::GetSettingProfiles() const
	{
		return _setting_profiles;
	}
	string BuildServer::GetActiveProfile() const
	{
		Assert(_active_settings);
		for (auto& profile : _setting_profiles)
		{
			if (profile.second == _active_settings)
				return profile.first;
		}
		return "";
	}


} // namespace sb
