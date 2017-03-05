// Copyright 2008-2014 Simon Ekström

#include <foundation/Common.h>

#include "../Event.h"

#include <process.h>


namespace edge
{

//-------------------------------------------------------------------------------
Event::Event(bool manual_reset, bool initial_state) : _state(initial_state), _auto_reset(!manual_reset)
{
	int res = pthread_mutex_init(&_mutex, NULL);
	Assert(res == 0);
	res = pthread_cond_init(&_cond, NULL);
	Assert(res == 0);
}
Event::~Event()
{
	pthread_cond_destroy(&_cond);
	pthread_mutex_destroy(&_mutex);
}

void Event::Set()
{
	pthread_mutex_lock(&_mutex);
	_state = true;

	int res = pthread_cond_broadcast(&_cond);
	Assert(res == 0);

	pthread_mutex_unlock(&_mutex);
}

void Event::Reset()
{
	pthread_mutex_lock(&_mutex);
	_state = false;

	pthread_mutex_unlock(&_mutex);
}

void Event::Wait()
{
	pthread_mutex_lock(&_mutex);
	
	while(!_state)
	{
		int res = pthread_cond_wait(&_cond, &_mutex);
		Assert(res == 0);
	}
	
	if(_auto_reset)
		_state = false;

	pthread_mutex_unlock(&_mutex);
}

bool Event::Wait(uint32_t milliseconds)
{
	struct timespec timeout;
	timeout.tv_sec  = milliseconds / 1000;
	timeout.tv_nsec = (milliseconds % 1000)*1000000;

	int res = 0;

	pthread_mutex_lock(&_mutex);
	
	if(_state)
	{
		if(_auto_reset)
			_state = false;
	}
	else
	{
		res = pthread_cond_timedwait(&_cond, &_mutex, &timeout);
	}

	pthread_mutex_unlock(&_mutex);
	return res == 0;
}

//-------------------------------------------------------------------------------


}; // namespace edge

