// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDER_SHADERDATABASE_H__
#define __BUILDER_SHADERDATABASE_H__

#include "CompilerSystem.h"

namespace sb
{

	/// Database keeping track of all needed shaded permutations
	class ShaderDatabase : NonCopyable
	{
	public:
		class Shader
		{
		public:
			struct Permutation
			{
				Permutation() {}

				Permutation(const vector<string> _options) : options(_options)
				{
				}

				vector<string> options;
			};
			typedef map<string, Permutation> PermutationMap;

		public:
			Shader(const FilePath& source_path, const ConfigValue& shader_cfg);
			~Shader();

			/// Preload a permutation of this shader, this permutation will then be compiled when
			///		the shader gets compiled.
			/// @param options Options for the permutation
			/// @return True if this shader can complete this request, false if it can't (In case of invalid shader options)
			bool PreloadPermutation(const vector<string>& options);

			PermutationMap& GetPermutations();


			/// Builds a name from the specified options
			/// @param name String that is going to hold the new name
			void BuildName(const vector<string>& options, string& name) const;

			/// @return True if the specified options are valid for a permutation of this shader
			bool IsValidOptions(const vector<string>& options) const;

			void Update(const ConfigValue& shader_cfg);

			void SetDirty(bool dirty);
			bool IsDirty() const;

			const string& GetName() const;
			const AssetSource& GetSource() const;

			const ConfigValue& GetConfig() const;


		private:
			const Shader& operator=(const Shader&) { return *this; }

			string _name;
			AssetSource _source;
			ConfigValue _shader_cfg;
			PermutationMap _permutations;

			bool _dirty;
		};

	public:
		ShaderDatabase(FileSource* file_source);
		~ShaderDatabase();

		/// Preloads a shader with the specified path
		/// @param shaderName Name of the shader (E.g. "shaders/test_shader")
		/// @return True if shader was successfully loaded or already loaded, false if load failed
		bool PreloadShader(const char* shader_name);

		/// Preloads a shader permutation with the specified options
		///		If the specified shader doesn't not exist in the database it will try to load it.
		/// @param shaderName Name of the shader (E.g. "shaders/test_shader")
		/// @return True if preload was successful, false if it failed
		bool PreloadPermutation(const char* shader_name, const vector<string>& options);

		/// @brief Inserts a shader into the database.
		void InsertShader(const FilePath& source_path, const ConfigValue& shader_cfg);

		/// Save database to file
		void Save();
		/// Load database from file
		bool Load();

		void Clear();

		/// Returns the shader with the specified name, will assert if the shader was not found
		/// @sa HasShader
		Shader& GetShader(const char* shader_name);

		/// @return True if shader exist in database, false if not
		bool HasShader(const char* shader_name) const;


		/// @return True if database contains any shaders that needs to be recompiled
		bool IsDirty() const;

		/// Adds all dirty shader sources to the given vector
		void GetDirtyShaders(vector<AssetSource>& sources) const;

	private:


	private:
		FileSource* _file_source;

		map<StringId64, Shader> _shaders;

	};

} // namespace sb


#endif // __BUILDER_SHADERDATABASE_H__

