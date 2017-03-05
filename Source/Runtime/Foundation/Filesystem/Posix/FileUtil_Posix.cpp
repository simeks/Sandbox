// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "../FileUtil.h"

#include <glob.h>

namespace sb
{

void file_util::FindFiles(const char* path, Vector<String>& files)
{
	glob_t g;
	glob(path, GLOB_TILDE|GLOB_MARK, NULL, &g);
	for(int i = 0; i < g.gl_matchc; ++i)
	{
		char* entry = g.gl_pathv[i];
		if(entry[strlen(g.gl_pathv[i])-1] != '/') // Directories end with '/'
		{
			entry[strlen(g.gl_pathv[i])-1] = '\0'; // Stip the trailing '/' from the resulting path
			files.PushBack(String(entry));
		}
	}
	globfree(&g);
}

void file_util::FindDirectories(const char* path, Vector<String>& directories)
{
	glob_t g;
	glob(path, GLOB_TILDE|GLOB_MARK, NULL, &g);
	for(int i = 0; i < g.gl_matchc; ++i)
	{
		const char* entry = g.gl_pathv[i];
		if(entry[strlen(g.gl_pathv[i])-1] == '/') // Directories end with '/'
			directories.PushBack(String(entry));
	}
	globfree(&g);
}


} // namespace sb



