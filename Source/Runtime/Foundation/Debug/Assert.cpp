// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Assert.h"

namespace sb
{

	AssertRetval UtilAssertMsg(const char* msg, const char* file_name, unsigned int line)
	{

#ifdef SANDBOX_PLATFORM_WIN  
		char tmp[4096];

		sprintf_s(tmp, 4096, "%s:%u : %s", file_name, line, msg);

		logging::AssertMessage(tmp);

		int res = MessageBoxA(NULL, tmp, "Error", MB_ABORTRETRYIGNORE | MB_ICONERROR);

		switch (res)
		{
		case IDRETRY:
			return RET_DEBUGGER;
		case IDIGNORE:
			return RET_CONTINUE;
		default:
			//case IDABORT:
			exit(0);
		};

#elif SANDBOX_PLATFORM_MACOSX
		return RET_DEBUGGER; // TODO:
#endif


	}

} // namespace sb

