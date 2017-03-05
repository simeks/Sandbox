// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDER_DEPENDENCYDATABASE_H__
#define __BUILDER_DEPENDENCYDATABASE_H__

#include "CompilerSystem.h"

namespace sb
{

	class FileSource;
	class DependencyDatabase
	{
	public:
		/// @param file_source FileSource for the asset source folder
		DependencyDatabase(FileSource* file_source);
		~DependencyDatabase();

		void AddDependent(const char* resource_path, const char* dependent_path);
		void GetDependents(const char* resource_path, vector<string>& dependents);

		/// Updates the last modified time for a specific file 
		void UpdateModifiedTime(const char* resource_path, const FileTime& modified_time);

		/// Tries to retrieve the last modified time for the specified file
		/// @return True if file was found, false if not (meaning modified_time is useless).
		bool GetModifiedTime(const char* resource_path, FileTime& modified_time);


		/// Saves dependencies to a file on the disk
		void Save();

		/// Tries to load database from file
		/// @return True if load was successful, false if it failed
		bool Load();

		/// Clears the complete database
		void Clear();

	private:
		const DependencyDatabase& operator=(const DependencyDatabase&) { return *this; }

		struct Entry
		{
			Entry() {}
			vector<string> dependents;
			FileTime last_modified;
		};
		typedef map<string, Entry> DependencyMap;

		FileSource* _file_source;
		DependencyMap _dependencies;


	};

} // namespace sb



#endif // __BUILDER_DEPENDENCYDATABASE_H__

