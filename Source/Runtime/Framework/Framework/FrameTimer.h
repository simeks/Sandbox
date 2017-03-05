// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_FRAMETIMER_H__
#define __FRAMEWORK_FRAMETIMER_H__


namespace sb
{

	class FrameTimer
	{
	public:
		FrameTimer();
		~FrameTimer();

		/// Performs a tick on the timer.
		/// @return Elapsed time since last tick in seconds.
		float Tick();

		/// Prints time statistics to console.
		void PrintStats();


	private:
		double _last_frame_time;
		float _max_frame_time;
		float _min_frame_time;
		float _avg_frame_time;
		uint32_t _frame_count;

	};

} // namespace sb

#endif // __FRAMEWORK_FRAMETIMER_H__
