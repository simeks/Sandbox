// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Thread_posix.h"
#include "Thread.h"

#include <pthread.h>


/// @file Thread_posix.cpp
///	@brief Threading implementations for POSIX

namespace
{

	struct args {
	    int joined;
	    pthread_t td;
	    pthread_mutex_t mtx;
	    pthread_cond_t cond;
	    void **res;
	};

	static void *waiter(void *ap)
	{
	    struct args *args = (struct args *)ap;
	    pthread_join(args->td, args->res);
	    pthread_mutex_lock(&args->mtx);
	    pthread_mutex_unlock(&args->mtx);
	    args->joined = 1;
	    pthread_cond_signal(&args->cond);
	    return 0;
	}

	int pthread_timedjoin_np(pthread_t td, void **res, struct timespec *ts)
	{
	    pthread_t tmp;
	    int ret;
	    struct args args;
	    args.td = td;
	    args.res = res;

	    pthread_mutex_init(&args.mtx, 0);
	    pthread_cond_init(&args.cond, 0);
	    pthread_mutex_lock(&args.mtx);

	    ret = pthread_create(&tmp, 0, waiter, &args);
	    if (ret) return 1;

	    do ret = pthread_cond_timedwait(&args.cond, &args.mtx, ts);
	    while (!args.joined && ret != ETIMEDOUT);

	    pthread_cancel(tmp);
	    pthread_join(tmp, 0);

	    pthread_cond_destroy(&args.cond);
	    pthread_mutex_destroy(&args.mtx);

	    return args.joined ? 0 : ret;
	}

}

namespace sb
{

//-------------------------------------------------------------------------------
ThreadLocalPtr::ThreadLocalPtr()
{
	int res = pthread_key_create(&_key, NULL);
	Assert(res == 0); // Check if we successfully created the key
	Set(NULL);
}
ThreadLocalPtr::~ThreadLocalPtr()
{
	pthread_key_delete(_key);
}
void ThreadLocalPtr::Set(void* value)
{
	pthread_setspecific(_key, value);
}
void* ThreadLocalPtr::Get() const
{
	return pthread_getspecific(_key);
}

//-------------------------------------------------------------------------------
SimpleThread::SimpleThread()
	: _thread_handle(NULL),
	_runnable(NULL)
{
	_thread_payload.function = NULL;
	_thread_payload.params = NULL;
}
SimpleThread::~SimpleThread()
{
	if(_thread_handle)
	{
		pthread_detach(_thread_handle);
	}
}
//-------------------------------------------------------------------------------
bool SimpleThread::Join(uint32_t time_out)
{
	bool result = false;
	if(_thread_handle != 0)
	{
		if(time_out)
		{
			struct timespec spec;
			spec.tv_sec = time_out / 1000;
			spec.tv_nsec = (time_out % 1000) * 1000000;
			result = pthread_timedjoin_np(_thread_handle, NULL, &spec) == 0;
		}
		else
		{
			result = pthread_join(_thread_handle, NULL) == 0;
		}
	}
	if(result)
		_thread_handle = NULL;

	return true;
}
//-------------------------------------------------------------------------------
void SimpleThread::StartThread(void* (*func)(void *), void* arg_list)
{
	pthread_attr_t attr;
	int res = pthread_attr_init(&attr);
	Assert(res == 0);

	res = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	Assert(res == 0);
	res = pthread_create(&_thread_handle, &attr, func, arg_list);
	Assert(res == 0);

	res = pthread_attr_destroy(&attr);
	Assert(res == 0);
}

//-------------------------------------------------------------------------------
// Static methods

void* SimpleThread::RunRunnable(void* param)
{
	SimpleThread* thread = static_cast<SimpleThread*>(param);

	if(thread->_runnable)
		thread->_runnable->Run();

	thread->_thread_handle = NULL;
	return 0;
}
void* SimpleThread::RunFunction(void* param)
{
	SimpleThread* thread = static_cast<SimpleThread*>(param);

	if(thread->_thread_payload.function)
		thread->_thread_payload.function(thread->_thread_payload.params);
	
	thread->_thread_handle = NULL;
	return 0;
}

//-------------------------------------------------------------------------------

#ifdef SANDBOX_PLATFORM_MACOSX

long thread::InterlockedIncrement(long volatile* addend)
{
	return __sync_fetch_and_add(addend, 1) + 1;
}
long thread::InterlockedDecrement(long volatile* addend)
{
	return __sync_fetch_and_sub(addend, 1) - 1;
}
long thread::InterlockedExchangeAdd(long volatile* addend, long value)
{
	return __sync_fetch_and_add(addend, value);
}
unsigned long thread::InterlockedExchangeSubtract(unsigned long volatile* addend, unsigned long value)
{
	return __sync_fetch_and_sub(addend, value);
}
long thread::InterlockedCompareExchange(long volatile* dest, long exchange, long comparand)
{
	return __sync_val_compare_and_swap(dest, comparand, exchange);
}
long thread::InterlockedExchange(long volatile* dest, long value)
{
	return __sync_val_compare_and_swap(dest, *dest, value);
}

#endif

//-------------------------------------------------------------------------------


} // namespace sb

