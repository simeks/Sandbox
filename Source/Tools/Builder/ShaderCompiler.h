// Copyright 2008-2014 Simon Ekström

#ifndef __SHADERCOMPILER_H__
#define __SHADERCOMPILER_H__

#include "CompilerSystem.h"
#include "ShaderDatabase.h"

#include <Foundation/IO/InputBuffer.h>

namespace sb
{

	struct ShaderData;
	struct ShaderResourceReflection;
	class D3D11Platform;
	class ShaderCompiler : public CompilerSystem::Compiler
	{
	public:
		enum ShaderType
		{
			VERTEX_SHADER = 0,
			HULL_SHADER = 1,
			DOMAIN_SHADER = 2,
			GEOMETRY_SHADER = 3,
			PIXEL_SHADER = 4,
			COMPUTE_SHADER = 5
		};

		struct Macro
		{
			const char* name;
			const char* definition;
		};


		/// @brief File cache for shader files
		///	When a file is read for the first time it will be cached in memory, later when
		///		the file is requested again the file will already be in memory.
		/// All files in the memory will be cleared either when Clear is called or when this object
		///		is destructed.
		class ShaderFileCache
		{
		public:
			ShaderFileCache(FileSource* file_source);
			~ShaderFileCache();

			/// Starts adding every file that gets opened as a dependencies to the specified database
			void RecordDependencies(const FilePath& resource, DependencyDatabase* database);

			/// @param file File path relative to the specified base path.
			InputBuffer* Open(const char* file);

			// Clears the cache
			void Clear();

		private:
			const ShaderFileCache& operator=(ShaderFileCache&) { return *this; }

			FileSource* _file_source;

			map<StringId64, InputBuffer*> _files;

			DependencyDatabase* _dependency_database;
			FilePath _dependent_path;
		};


	public:
		ShaderCompiler(const ConfigValue& config);
		~ShaderCompiler();

		CompilerSystem::Result Compile(const FilePath& source_file, const FilePath& target_path, const CompilerSystem::CompilerContext& context);

	private:
		bool CompilePermutation(const ShaderDatabase::Shader& shader, const vector<string>& defines, const CompilerSystem::CompilerContext& context,
			D3D11Platform& platform, ShaderData& out_shader_data);

	};

} // namespace sb



#endif // __SHADERCOMPILER_H__

