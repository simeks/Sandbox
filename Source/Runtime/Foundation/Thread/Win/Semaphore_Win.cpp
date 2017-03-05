// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "../Semaphore.h"

#include <process.h>


namespace sb
{

	//-------------------------------------------------------------------------------
	Semaphore::Semaphore(int n)
	{
		// Max value = 0x7fffffff (MAXLONG)
		_handle = (void*)CreateSemaphore(NULL, n, 0x7fffffff, NULL);
	}
	Semaphore::~Semaphore()
	{
		CloseHandle(_handle);
	}

	void Semaphore::Set()
	{
		ReleaseSemaphore(_handle, 1, NULL);
	}

	void Semaphore::Wait()
	{
		WaitForSingleObject(_handle, INFINITE);
	}
	//-------------------------------------------------------------------------------

} // namespace sb


