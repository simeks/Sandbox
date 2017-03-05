// Copyright 2008-2014 Simon Ekström

#ifndef __PROFILER_H__
#define __PROFILER_H__

#define USE_PROFILER 1

#ifdef USE_PROFILER
#define MICROPROFILE_ENABLED 1
#define MICROPROFILE_UI 1
//#define MICROPROFILE_WEBSERVER 1
#include "microprofile.h"
#endif


#ifdef USE_PROFILER
#define PROFILER_SCOPE(name) MICROPROFILE_SCOPEI("Main", name, 0xFF329932)

#else
#define PROFILER_SCOPE(name)
#endif

namespace sb
{
namespace profiler
{
	class DrawCallback
	{
	public:
		virtual void DrawText(int nX, int nY, uint32_t nColor, const char* pText, uint32_t nNumCharacters) = 0;
		virtual void DrawBox(int nX, int nY, int nX1, int nY1, uint32_t nColor) = 0;
		virtual void DrawLine2D(uint32_t nVertices, float* pVertices, uint32_t nColor) = 0;
	};

	void Flip();
	void Draw(uint32_t width, uint32_t height);

	void ToggleDisplayMode();
	void TogglePause();

	void SetDrawCallback(DrawCallback* callback);
}
} // namespace sb


#endif // __PROFILER_H__
