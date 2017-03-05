// Copyright 2008-2014 Simon Ekström

#ifndef __ENGINE_VERSION_H__
#define __ENGINE_VERSION_H__

#include "EngineVersion.h"

#define STR2(x) #x
#define STR(x) STR2(x)
#define SANDBOX_VERSION_NAME "Sandbox-" STR(ENGINE_COMMIT_ID)

#if defined(SANDBOX_PLATFORM_WIN64)
#define SANDBOX_FULL_VERSION_NAME SANDBOX_VERSION_NAME " (Win64)"
#elif defined(SANDBOX_PLATFORM_WIN)
#define SANDBOX_FULL_VERSION_NAME SANDBOX_VERSION_NAME " (Win32)"
#else
#define SANDBOX_FULL_VERSION_NAME SANDBOX_VERSION_NAME
#endif

#endif // __ENGINE_VERSION_H__