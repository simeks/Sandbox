// Copyright 2008-2014 Simon Ekström

#ifndef __THREAD_LOCK_H__
#define __THREAD_LOCK_H__


namespace sb
{

	/// @brief Win32 specific mutex
	class Mutex
	{
	public:
		Mutex();
		~Mutex();

		void Lock();
		void Unlock();
		bool TryLock();

	private:
#ifdef SANDBOX_PLATFORM_WIN
		void* _handle;
#else
		pthread_mutex_t _mutex;
#endif
	};


#ifdef SANDBOX_PLATFORM_WIN

	/// @brief Critical section lock for win32
	class CriticalSection
	{
	public:
		CriticalSection();
		~CriticalSection();

		void Lock();
		void Unlock();
		bool TryLock();

	private:
		CRITICAL_SECTION _cs;
	};

#else
	typedef Mutex CriticalSection;

#endif


	/// @brief Scoped lock to simplify synchronization in a scope 
	template<typename T>
	class ScopedLock
	{
	public:
		ScopedLock(T& lock) : _lock(lock)
		{
			_lock.Lock();
		}
		~ScopedLock()
		{
			_lock.Unlock();
		}
	private:
		T& _lock;

		ScopedLock();
		ScopedLock(const ScopedLock&);
		ScopedLock& operator=(const ScopedLock&);
	};

} // namespace sb



#endif // __THREAD_LOCK_H__
