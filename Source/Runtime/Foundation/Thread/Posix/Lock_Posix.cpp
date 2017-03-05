// Copyright 2008-2014 Simon Ekström

#include <foundation/Common.h>

#include "../Lock.h"

#include <process.h>


namespace edge
{

//-------------------------------------------------------------------------------
Mutex::Mutex()
{
	pthread_mutexattr_t attr;
	int res = pthread_mutexattr_init(&attr);
	Assert(res == 0);

	res = pthread_mutex_init(&_mutex, &attr);
	Assert(res == 0);
	
	res = pthread_mutexattr_destroy(&attr);
	Assert(res == 0);
}
Mutex::~Mutex()
{
	pthread_mutex_destroy(&_mutex);
}

void Mutex::Lock()
{
	pthread_mutex_lock(&_mutex);
}

void Mutex::Unlock()
{
	pthread_mutex_unlock(&_mutex);
}

bool Mutex::TryLock()
{
	const int rc = pthread_mutex_trylock(&_mutex);
	if (rc == 0)
	{
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------------


}; // namespace edge

