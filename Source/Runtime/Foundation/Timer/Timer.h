// Copyright 2008-2014 Simon Ekström

#ifndef _TIMER_H
#define _TIMER_H



namespace sb
{

	namespace timer
	{
		void Initialize();

		/// Returns the number of ticks since timer initialization.
		uint64_t StartTickCount();

		/// Returns the applications current tick count.
		uint64_t TickCount();

		/// Returns elapsed seconds since timer initialization.
		double Seconds();

		/// Returns the number of seconds per timer tick.
		double SecondsPerTick();

	};


} // namespace sb



#endif // _TIMER_H
