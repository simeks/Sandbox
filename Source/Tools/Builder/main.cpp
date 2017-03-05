// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "BuildServer.h"
#include "BuildProgressDialog.h"
#include "BuildServerDialog.h"

#include <Foundation/Filesystem/FileUtil.h>

/// Returns the path to the directory for this binary
void GetBinariesDir(char* bindir, int size)
{
	char* buffer = NULL;
	bindir[0] = 0;

	if (::GetModuleFileNameA(NULL, bindir, size))
	{
		buffer = strrchr(bindir, '\\');
		if (*buffer)
		{
			*(buffer + 1) = '\0';
		}
	}
}

bool ParseCommandLine(int argc, char* argv[], string& source_path, string& target_path, bool& server_mode)
{
	int i = 1;
	while (i < argc)
	{
		const char* str = argv[i];
		if (str[0] != '-')
			break;

		if (str[1] == '-') // '--'
		{
			if ((strcmp(str + 2, "source") == 0) && (i + 1 < argc))
			{
				source_path = argv[++i];
			}
			else if ((strcmp(str + 2, "target") == 0) && (i + 1 < argc))
			{
				target_path = argv[++i];
			}
			else if ((strcmp(str + 2, "server") == 0))
			{
				server_mode = true;
			}
		}
		else // Single '-'
		{
			if (str[1] == 's' && (i + 1 < argc))
			{
				source_path = argv[++i];
			}
			else if (str[1] == 't' && (i + 1 < argc))
			{
				target_path = argv[++i];
			}
		}

		++i;
	}
	return true;
}

#if 0
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#else
int main()
#endif
{
	using namespace sb;

	memory::Initialize();
	{
		
		char base_path[MAX_PATH];
		GetBinariesDir(base_path, MAX_PATH);

		string log_path;

		file_util::BuildOSPath(log_path, base_path, "builder.log");

		logging::Initialize(log_path.c_str());

		BuildUICallback* callback = 0;

		bool server_mode = false;
		string source_path = "Content";
		string target_path = "Binaries/Content";

		ParseCommandLine(__argc, __argv, source_path, target_path, server_mode);

		// Convert the data paths relative to the working directory
		char working_dir[FILENAME_MAX];
		GetCurrentDirectory(sizeof(working_dir), working_dir);

		BuilderParams params;
		params.base_path = base_path;
		params.force_recompile = false;
		params.server = server_mode;

		file_util::BuildOSPath(params.source_path, working_dir, source_path);
		file_util::BuildOSPath(params.target_path, working_dir, target_path);

		BuildServer builder(params);

		if (params.server)
		{
			callback = new BuildServerDialog(&builder);
		}
		else
		{
			callback = new BuildProgressDialog();
		}

		builder.SetCallback(callback);

		SimpleThread builder_thread;
		builder_thread.Start(&builder);

		while (builder_thread.IsRunning())
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					builder.Stop();
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			Sleep(1);
		}

		if (callback)
		{
			delete callback;
		}

	}
	memory::Shutdown();
	logging::Shutdown();
}
