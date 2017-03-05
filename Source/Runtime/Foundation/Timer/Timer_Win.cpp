// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Timer.h"


namespace sb
{

	namespace
	{
		uint64_t g_ticks_per_second = 0;
		double g_seconds_per_tick = 0;

		uint64_t g_start_tick_count = 0;

		bool g_initialized = false;
	};

	void timer::Initialize()
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		Assert(freq.QuadPart != 0); // Make sure system actually supports high-res counter
		g_ticks_per_second = freq.QuadPart;
		g_seconds_per_tick = 1.0 / (double)freq.QuadPart;

		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		g_start_tick_count = counter.QuadPart;

		g_initialized = true;
	}

	uint64_t timer::StartTickCount()
	{
		Assert(g_initialized);
		return g_start_tick_count;
	}
	uint64_t timer::TickCount()
	{
		Assert(g_initialized);
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);

		return counter.QuadPart;
	}
	double timer::Seconds()
	{
		Assert(g_initialized);
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);

		return double(counter.QuadPart - g_start_tick_count) * g_seconds_per_tick;
	}

	double timer::SecondsPerTick()
	{
		return g_seconds_per_tick;
	}

} // namespace sb

