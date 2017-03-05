// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDER_ASSETCOMPILER_H_
#define __BUILDER_ASSETCOMPILER_H_

#include <Foundation/Filesystem/FileSystem.h>
#include <Foundation/Filesystem/FilePath.h>
#include <Foundation/Container/ConfigValue.h>

#include "Settings.h"

namespace sb
{

	class BuildServer;
	class ShaderDatabase;
	class DependencyDatabase;
	class BuildUICallback;
	struct BuildSettings;

	struct AssetSource
	{
		AssetSource(const char* path)
			: source_path(path)
		{
			source_path.SetSeparator('/'); // All assets use '/' as separator.
			source_type = source_path.Extension();
		}

		FilePath source_path;
		string source_type;
	};

	class CompilerSystem
	{
	public:
		/// Compile result
		enum ResultEnum { SUCCESSFUL, FAILED };
		typedef uint8_t Result;

		struct CompilerContext
		{
			CompilerContext(FileSource* source, FileSource* target, DependencyDatabase* dependency_db,
				ShaderDatabase* shader_db, BuildSettings* settings)
				: asset_source(source),
				asset_target(target),
				dependency_database(dependency_db),
				shader_database(shader_db),
				settings(settings)
			{
			}

			FileSource* asset_source;
			FileSource* asset_target;

			DependencyDatabase* dependency_database;
			ShaderDatabase* shader_database;

			BuildSettings* settings;
		};

		class Compiler
		{
		public:
			Compiler(const ConfigValue& config);
			virtual ~Compiler() { }

			/// Compiles the given assets and stores it in memory allocated with the specified allocator
			/// @return Pointer to the memory holding the compiled asset 
			virtual Result Compile(const FilePath& source_file, const FilePath& target_file, const CompilerContext& context) = 0;

			/// @return True if the given asset needs to be compiled, false if not
			virtual bool NeedCompile(const FilePath& source_file, const FilePath& target_file, const CompilerContext& context);

			const string& GetType() const;
			const string& GetSourceType() const;

			const string& GetLastError() const;

		protected:
			const Compiler& operator=(const Compiler&) { return *this; }

			bool WriteAsset(FileSource* asset_target, const FilePath& path, const uint8_t* data, uint32_t len);

			void SetError(const char* msg);

			string _type;
			string _source_type;

			string _target_path;
			string _source_path;

			string _last_error;
		};


	public:
		CompilerSystem(BuildServer* builder, FileSource* asset_source, FileSource* asset_target,
			DependencyDatabase* dependency_db, ShaderDatabase* shader_db, BuildSettings* settings);
		~CompilerSystem();

		/// Compiles the given asset sources
		/// @param force If true, all given files will be forced to recompiled even if not needed.
		void Compile(AssetSource* sources, uint32_t num, bool force = false, BuildUICallback* callback = NULL);

		/// Assigns a compiler to a specific source type
		void RegisterCompiler(const char* source_type, Compiler* compiler);
		void UnregisterCompiler(Compiler* compiler);

		void SetBuildSettings(BuildSettings* settings);

		void AddIgnoreAsset(const char* asset_name);

	private:
		BuildServer* _builder;

		map<StringId32, Compiler*> _compilers;
		vector<string> _ignores;

		FileSource* _asset_source;
		FileSource* _asset_target;

		DependencyDatabase* _dependency_database;
		ShaderDatabase* _shader_database;

		BuildSettings* _active_settings;

	};

} // namespace sb



#endif // __BUILDER_ASSETCOMPILER_H_

