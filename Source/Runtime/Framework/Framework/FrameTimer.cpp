// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "FrameTimer.h"

#include <Foundation/Timer/Timer.h>


namespace sb
{

	FrameTimer::FrameTimer()
		: _max_frame_time(0.0f),
		_min_frame_time(FLT_MAX),
		_avg_frame_time(0.0f),
		_frame_count(0)
	{
		_last_frame_time = timer::Seconds();
	}
	FrameTimer::~FrameTimer()
	{
	}

	float FrameTimer::Tick()
	{
		double current_time = timer::Seconds();
		float dtime = float(current_time - _last_frame_time);
		_last_frame_time = current_time;

		if (_frame_count > 50) // Skip first frames
		{
			_max_frame_time = Max(dtime, _max_frame_time);
			_min_frame_time = Min(dtime, _min_frame_time);
		}

		if (_avg_frame_time == 0.0f)
		{
			_avg_frame_time = dtime;
		}
		else
		{
			_avg_frame_time = (_avg_frame_time + dtime) / 2.0f;
		}

		++_frame_count;

		return dtime;
	}
	void FrameTimer::PrintStats()
	{
		logging::Info("Frame time Avg: %f ms, Max: %f ms, Min: %f ms", (_avg_frame_time * 1000.0f), (_max_frame_time * 1000.0f), (_min_frame_time * 1000.0f));
	}

} // namespace sb
