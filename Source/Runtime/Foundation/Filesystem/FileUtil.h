// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_FILEUTIL_H__
#define __FOUNDATION_FILEUTIL_H__

#ifdef _WIN32
#define PATH_SEPARATOR '\\'

#else
#define PATH_SEPARATOR '/'

#endif

namespace sb
{

	class FileSource;

	/// @brief File system utilities
	namespace file_util
	{
		/// @brief Builds a full os-path from the given parameters 
		///	@param base		Base path (E.g. "C:/Game/") 
		///	@param relative	Relative path (E.g. "content/")
		void BuildOSPath(string& out, const string& base, const string& relative);

		/// @brief Fixes all the slashes in a path 
		void FixSlashes(char* path);

		/// @brief Fixes all the slashes in a path 
		void FixSlashes(string& path);

		/// @brief Searches through the specified folder and the whole subtree adding all found files to outfiles.
		///	The file names stored in outfiles will be the file path relative to the specified file_source.
		void FindFilesRecursive(FileSource* file_source, const char* dir, vector<string>& outfiles);

		/// @brief Finds all files matching the specified patterns and puts them in the vector files.
		void FindFiles(const char* path, vector<string>& files);

		/// @brief Finds all directories matching the specified patterns and puts them in the vector files.
		void FindDirectories(const char* path, vector<string>& directories);

	}; // namespace file_util

} // namespace sb


#endif // __FOUNDATION_FILEUTIL_H__
