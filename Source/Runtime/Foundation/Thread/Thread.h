// Copyright 2008-2014 Simon Ekström

#ifndef __THREAD_H__
#define __THREAD_H__

#include "Lock.h"
#include "Semaphore.h"
#include "Event.h"


/// @file Thread.h
///	@brief Threading utilities


namespace sb
{

	/// @brief Thread local storage
	class ThreadLocalPtr
	{
	public:
		ThreadLocalPtr();
		~ThreadLocalPtr();

		void	Set(void* value);
		void*	Get() const;

	private:
#ifdef SANDBOX_PLATFORM_WIN
		DWORD _tls_index;
#else
		pthread_key_t _key;
#endif
	};



	/// @brief Thread runnable 
	class Runnable
	{
	public:
		virtual ~Runnable() {}
		virtual void Run() = 0;
	};

	class SimpleThread
	{
	public:
		typedef void(*ThreadFunction)(void*);

		SimpleThread();
		virtual ~SimpleThread();


		/// @brief Starting the thread with a runnable object
		virtual void Start(Runnable* runnable)
		{
			_runnable = runnable;

			StartThread(RunRunnable, this);
		}

		/// @brief Starting the thread with a function and parameters
		virtual void Start(ThreadFunction thread_function, void* params = NULL)
		{
			_thread_payload.function = thread_function;
			_thread_payload.params = params;

			StartThread(RunFunction, this);
		}

		/// Waits for this thread to finish execution
		/// @param time_out Maximum time (In milliseconds) to wait, waits indefinitely if set to zero.
		/// @return True if the thread finished.
		bool Join(uint32_t time_out = 0);

		/// @brief Is the thread running?
		bool IsRunning() const { return (_thread_handle != NULL); }

	protected:
		struct ThreadPayload
		{
			ThreadFunction function;
			void* params;
		};

#ifdef SANDBOX_PLATFORM_WIN
		HANDLE _thread_handle;
		uint32_t _thread_id;

#else
		pthread_t _thread_handle;
#endif

		ThreadPayload _thread_payload;
		Runnable* _runnable;

	private:
		SimpleThread(const SimpleThread&);
		void operator=(const SimpleThread&);


#ifdef SANDBOX_PLATFORM_WIN
		void StartThread(unsigned(__stdcall *func)(void *), void* arg_list);

		static unsigned int __stdcall RunRunnable(void* param);
		static unsigned int __stdcall RunFunction(void* param);

#else
		void StartThread(void* (*func)(void *), void* arg_list);

		static void* RunRunnable(void* param);
		static void* RunFunction(void* param);
#endif
	};



	namespace thread
	{
		long InterlockedIncrement(long volatile* addend);
		long InterlockedDecrement(long volatile* addend);

		/// @return The initial value of the addend parameter.
		long InterlockedExchangeAdd(long volatile* addend, long value);
		unsigned long InterlockedExchangeSubtract(unsigned long volatile* addend, unsigned long value);

		/// @return The initial value of the dest parameter.
		long InterlockedCompareExchange(long volatile* dest, long exchange, long comparand);

		/// @return Initial value of dest.
		long InterlockedExchange(long volatile* dest, long value);

		int64_t InterlockedIncrement64(int64_t volatile* addend);
		int64_t InterlockedDecrement64(int64_t volatile* addend);

		int64_t InterlockedExchangeAdd64(int64_t volatile* addend, int64_t value);

	}; // namespace thread

} // namespace sb



#endif // __THREAD_H__
