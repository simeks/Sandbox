// Copyright 2008-2014 Simon Ekström

#ifndef __THREAD_SEMAPHORE_H__
#define __THREAD_SEMAPHORE_H__



namespace sb
{

	/// @brief Semaphore
	class Semaphore
	{
	public:
		/// @brief Constructor
		///
		///	@param n Start value, must be greater than zero.
		Semaphore(int n = 0);
		~Semaphore();

		/// @brief Increase semaphores value by one
		void Set();

		/// @brief Wait indefinitely for the semaphore to become signalled
		///
		///	Waits for semaphore to become signalled, than decrements the semaphores value by one.
		void Wait();



	private:
#ifdef SANDBOX_PLATFORM_WIN
		void*	_handle;
#else
		int _n;
		pthread_mutex_t _mutex;
		pthread_cond_t _cond;
#endif
	};

} // namespace sb



#endif // __THREAD_SEMAPHORE_H__
