// Copyright 2008-2014 Simon Ekström

#include <Framework/Common.h>

#include <Framework/Framework/GameFramework.h>
#include <Framework/Framework/Game.h>

#include <Foundation/Timer/Timer.h>
#include <Foundation/Filesystem/FileUtil.h>
#include <Foundation/Profiler/Profiler.h>

#include <Engine/Version.h>

using namespace sb;

IGame* CreateGame(GameFramework& framework);
void DestroyGame(IGame* game);



#ifdef SANDBOX_PLATFORM_WIN
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
#endif



bool ParseCommandLine(int argc, char* argv[], char* data_path, int path_size)
{
	int i = 1;
	while (i < argc)
	{
		const char* str = argv[i];
		if (str[0] != '-')
			break;

		if (str[1] == '-') // '--'
		{
			if ((strcmp(str + 2, "data") == 0) && (i + 1 < argc))
			{
				strcpy_s(data_path, path_size, argv[++i]);
			}
		}
		else // Single '-'
		{
			if (str[1] == 'd' && (i + 1 < argc))
			{
				strcpy_s(data_path, path_size, argv[++i]);
			}
		}

		++i;
	}
	return true;
}



#ifdef SANDBOX_PLATFORM_WIN
int WINAPI WinMain(HINSTANCE hInstanceIn, HINSTANCE, LPSTR, int)
#else
int main(int argc, char* argv[])
#endif
{
	MicroProfileOnThreadCreate("Main");
	MicroProfileSetEnableAllGroups(true);

	timer::Initialize();

	// Initialize our heap
	memory::Initialize();

	{
		char base_path[MAX_PATH];
		GetBinariesDir(base_path, MAX_PATH);

		string log_path;

#ifdef SANDBOX_BUILD_DEBUG
		file_util::BuildOSPath(log_path, base_path, "sandbox_log_debug.txt");
#elif SANDBOX_BUILD_PRODUCTION
		file_util::BuildOSPath(log_path, base_path, "sandbox_log_production.txt");
#else
		file_util::BuildOSPath(log_path, base_path, "sandbox_log.txt");
#endif

		logging::Initialize(log_path.c_str());
#ifdef SANDBOX_DEVELOPMENT
		logging::Info("*** Running development version ***");
#endif
		logging::Info("Version: %s", SANDBOX_FULL_VERSION_NAME);


		char data_path[MAX_PATH];
		strcpy(data_path, "../Content");

		ParseCommandLine(__argc, __argv, data_path, MAX_PATH);

		GameFramework::Params params;
		params.base_directory = base_path;
		params.data_directory = data_path;
		params.hinstance = hInstanceIn;

		GameFramework framework(params, CreateGame, DestroyGame);
		framework.Run();
	}

	memory::Shutdown();

	logging::Shutdown();

	MicroProfileShutdown();
}

