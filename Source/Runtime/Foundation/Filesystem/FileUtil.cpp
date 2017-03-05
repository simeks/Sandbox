// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "FileUtil.h"
#include "FileSource.h"
#include "FilePath.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	void file_util::BuildOSPath(string& out, const string& base, const string& relative)
	{
		out.clear();
		out = base;

		int i = (uint32_t)base.size() - 1;
		if (out[i] != '\\' && out[i] != '/')
			out += '/';

		out += relative;
		FixSlashes(out);
	}
	//-------------------------------------------------------------------------------
	void file_util::FixSlashes(char* path)
	{
		while (*path)
		{
			if (*path == '\\' || *path == '/')
			{
				*path = PATH_SEPARATOR;
			}
			path++;
		}
	}
	void file_util::FixSlashes(string& path)
	{
		char* _path = &path[0];
		while (*_path)
		{
			if (*_path == '\\' || *_path == '/')
			{
				*_path = PATH_SEPARATOR;
			}
			_path++;
		}
	}
	//-------------------------------------------------------------------------------

	void file_util::FindFilesRecursive(FileSource* file_source, const char* dir, vector<string>& outfiles)
	{
		vector<string> directories;
		vector<string> files;

		file_source->FindFiles(dir, files);

		FilePath file_path;
		// Iterate through found files and append the directory path before pushing them to the outfiles vector
		for (auto& f : files)
		{
			file_path = dir;
			file_path = file_path.Directory(); // Trim *-character
			file_path += f;

			outfiles.push_back(file_path.Get());

			file_path.Clear();
		}

		file_source->FindDirectories(dir, directories);
		for (auto& d : directories)
		{
			file_path = dir;
			file_path = file_path.Directory(); // Trim *-character
			file_path += d;
			file_path += PATH_SEPARATOR;
			file_path += '*'; // Add wildcard for when searching 

			FindFilesRecursive(file_source, file_path.c_str(), outfiles);
		}
	}


	//-------------------------------------------------------------------------------

} // namespace sb



