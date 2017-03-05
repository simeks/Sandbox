// Copyright 2008-2014 Simon Ekström


#ifndef _PLATFORM_H
#define _PLATFORM_H

#ifdef SANDBOX_PLATFORM_WIN
#include "Platform_Win.h"
#elif SANDBOX_PLATFORM_MACOSX
#include "Platform_Mac.h"
#endif



#endif // _PLATFORM_H
