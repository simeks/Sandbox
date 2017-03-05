// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_FILESOURCE_H__
#define __FOUNDATION_FILESOURCE_H__

#include "FileStream.h"

#include <list>

namespace sb
{

	struct FileTime
	{
		FileTime();

		bool operator==(const FileTime& other) const;
		bool operator!=(const FileTime& other) const;
		bool operator<(const FileTime& other) const;
		bool operator>(const FileTime& other) const;
		bool operator<=(const FileTime& other) const;
		bool operator>=(const FileTime& other) const;

		uint32_t low;
		uint32_t high;
	};

	class FileSystem;

	/// @brief File source for reading files from directories
	class FileSource
	{
		FileSystem* _file_system;
		string _path;

		const FileSource& operator=(const FileSource&) { return *this; }
	public:
		/// @brief Constructor
		///	@param Path for this directory
		FileSource(FileSystem* file_system, const char* path);
		~FileSource();

		/// @brief Tries to open a file with the specified flags
		///	@sa File::FileMode
		FileStreamPtr OpenFile(const char* file_path, const File::FileMode mode);


		/// Creates a new directory with the specified name
		void MakeDirectory(const char* path);


		/// Finds all files matching the specified path (Wildcards allowed) and adds
		///		the files name to the given array.
		/// @param files Vector for the result
		/// @remark Does not return any found directories
		/// @sa FindDirectories
		void FindFiles(const char* path, vector<string>& files) const;

		/// Finds all directories matching the specified path (Wildcards allowed) and adds
		///		the directory name to the given array.
		/// @param directories Vector for the result
		/// @remark Does not return any found files
		/// @sa FindFiles	
		void FindDirectories(const char* path, vector<string>& directories) const;

		/// Tries to retrieve the time when the specified file was last modified.
		/// @param file_path Path to the file
		/// @param file_time A structure which will hold the received time
		/// @return True if the operation succeeded, false if it failed.
		bool LastModifiedTime(const char* file_path, FileTime& file_time) const;


		const string& GetPath() const;
		string GetFullPath() const;
	};

} // namespace sb


#endif // __FOUNDATION_FILESOURCE_H__

