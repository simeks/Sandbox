// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "../FileUtil.h"

namespace sb
{

	void file_util::FindFiles(const char* path, vector<string>& files)
	{
		WIN32_FIND_DATA fd;
		HANDLE fh;

		fh = FindFirstFile(path, &fd);
		if (fh == INVALID_HANDLE_VALUE)
			return;

		do
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // Not a directory
			{
				files.push_back(fd.cFileName);
			}

		} while (FindNextFile(fh, &fd) != 0);

		FindClose(fh);
	}

	void file_util::FindDirectories(const char* path, vector<string>& directories)
	{
		WIN32_FIND_DATA fd;
		HANDLE fh;

		fh = FindFirstFile(path, &fd);
		if (fh == INVALID_HANDLE_VALUE)
			return;

		do
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if ((strcmp(fd.cFileName, "..") != 0) && (strcmp(fd.cFileName, ".") != 0))
					directories.push_back(fd.cFileName);
			}

		} while (FindNextFile(fh, &fd) != 0);

		FindClose(fh);
	}

} // namespace sb

