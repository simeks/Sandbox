// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDSERVER_H__
#define __BUILDSERVER_H__

#include "CompilerSystem.h"
#include "Settings.h"
#include <Foundation/Container/ConfigValue.h>
#include <Foundation/Thread/Thread.h>

#include <queue>

namespace sb
{

	class BuildUICallback;
	struct BuilderParams
	{
		BuilderParams() : server(false), force_recompile(false)
		{
		}

		string base_path;
		string source_path;
		string target_path;

		string relay_host;
		bool server;			///< Specifies if the builder should run in server mode
		bool force_recompile;	///< Specifies if we initially should recompile all assets

	};

	class StringIdRepository;
	class DirectoryWatcher;
	class ConsoleServer;
	class BuildServer : public Runnable
	{
	public:
		BuildServer(const BuilderParams& params);
		~BuildServer();

		void SetCallback(BuildUICallback* callback);

		void Run();
		void Stop();


		/// Queues a full rebuild.
		///	This is safe to call from main thread.
		void QueueFullRebuild();

		/// Queues a scan.
		///	This is safe to call from main thread.
		void QueueScan();

		/// Changes the current settings profile to the specified.
		///	This is safe to call from main thread.
		void SetActiveProfile(const string& profile);

		bool IsStopping() const;

		/// Rebuilds the whole source directroy
		/// @remark Do not call if not running in build server thread.
		void FullRebuild();

		/// Rebuilds any outdated assets
		/// @remark Do not call if not running in build server thread.
		void PartialRebuild();


		const map<string, BuildSettings*>& GetSettingProfiles() const;
		string GetActiveProfile() const;

	private:
		/// Scans the whole source directory and appends all source files
		void ScanSourceDirectory(vector<AssetSource>& sources);

		void RegisterCompilers(const ConfigValue& compilers);

		FileSource* _source;
		FileSource* _target;
		FileSystem _file_system;

		DirectoryWatcher* _dir_watcher;

		BuilderParams _params;
		BuildUICallback* _callback;

		map<string, BuildSettings*> _setting_profiles;
		BuildSettings* _active_settings;
		// Queued settings, this is used for temporary stored queued setting profiles set from the main thread.
		string _queued_profile;

		CompilerSystem* _compiler_system;
		vector<CompilerSystem::Compiler*> _compilers;

		DependencyDatabase*	_dependency_database;
		ShaderDatabase*	_shader_database;
		StringIdRepository* _string_id_repository;

		bool _stop;

		enum Action
		{
			FULL_REBUILD, // Starts doing a full build, forcing to compile all assets in the source folder
			SCAN, // Scans the source folders and builds any outdated assets
			CHANGE_PROFILE // Changes the settings profile
		};

		CriticalSection _action_queue_lock;
		std::queue<Action> _action_queue;

	};

} // namespace sb


#endif // __BUILDSERVER_H__


