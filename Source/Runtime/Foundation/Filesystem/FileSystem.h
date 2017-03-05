// Copyright 2008-2014 Simon Ekström

#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "File.h"
#include "FileSource.h"

namespace sb
{

	/// @brief FileSystem
	///	TODO: Thread-safety
	class FileSystem
	{
	public:
		enum PathAddFlags
		{
			ADD_TO_HEAD,	///< Search path will be added to the head (Searched first)
			ADD_TO_TAIL		///< Search path will be added to the tail (Searched last)
		};

	private:
		deque<FileSource*> _file_sources;

		string _base_path;

	public:
		FileSystem(const char* base_path);
		~FileSystem();


		/// @brief Tries to open a file with the specified flags
		///		This will search through all open file sources
		///	@remark The file must later be closed by calling CloseFile
		///	@sa File::FileMode
		///	@sa CloseFile
		FileStreamPtr OpenFile(const char* file_path, const File::FileMode mode);

		/// @brief Adds a search path where we will look for files.
		FileSource* OpenFileSource(const char* path, const PathAddFlags flags = ADD_TO_TAIL);

		/// @brief Removes a search path 
		void CloseFileSource(FileSource* source);
		void CloseFileSource(const char* path);

		/// @brief Creates a new directory
		/// @param path Directory to create, path can be either relative to the base path or absolute
		void MakeDirectory(const char* path);

		/// @brief Returns the base filepath for this filesystem
		const string& GetBasePath() const;

	};

} // namespace sb


#endif // __FILESYSTEM_H__
