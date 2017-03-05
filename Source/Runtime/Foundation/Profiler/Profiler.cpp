// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Profiler.h"

namespace sb
{
namespace
{
	profiler::DrawCallback* _callback = nullptr;
}

void profiler::Flip()
{
	MicroProfileFlip();
}
void profiler::Draw(uint32_t width, uint32_t height)
{
	MicroProfileDraw(width, height);
}
void profiler::ToggleDisplayMode()
{
	MicroProfileToggleDisplayMode();
}
void profiler::TogglePause()
{
	MicroProfileTogglePause();
}
void profiler::SetDrawCallback(DrawCallback* callback)
{
	_callback = callback;
}

} // namespace sb

void MicroProfileDrawText(int nX, int nY, uint32_t nColor, const char* pText, uint32_t nNumCharacters)
{
	if (sb::_callback)
	{
		sb::_callback->DrawText(nX, nY, nColor, pText, nNumCharacters);
	}
}
void MicroProfileDrawBox(int nX, int nY, int nX1, int nY1, uint32_t nColor, MicroProfileBoxType)
{
	if (sb::_callback)
	{
		sb::_callback->DrawBox(nX, nY, nX1, nY1, nColor);
	}
}
void MicroProfileDrawLine2D(uint32_t nVertices, float* pVertices, uint32_t nColor)
{
	if (sb::_callback)
	{
		sb::_callback->DrawLine2D(nVertices, pVertices, nColor);
	}
}

uint32_t MicroProfileGpuInsertTimeStamp()
{
	return 0;
}
uint64_t MicroProfileGpuGetTimeStamp(uint32_t )
{
	return 0;
}
uint64_t MicroProfileTicksPerSecondGpu()
{
	return 1000000000ll;
}

