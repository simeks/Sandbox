// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "../Lock.h"

#include <process.h>



namespace sb
{

	//-------------------------------------------------------------------------------

	Mutex::Mutex()
	{
		_handle = CreateMutex(NULL, FALSE, NULL);
	}
	Mutex::~Mutex()
	{
		CloseHandle(_handle);
	}

	void Mutex::Lock()
	{
		WaitForSingleObject(_handle, INFINITE);
	}

	void Mutex::Unlock()
	{
		ReleaseMutex(_handle);
	}

	bool Mutex::TryLock()
	{
		return WaitForSingleObject(_handle, 0) != WAIT_TIMEOUT;
	}

	//-------------------------------------------------------------------------------
	CriticalSection::CriticalSection()
	{
		InitializeCriticalSection(&_cs);
	}
	CriticalSection::~CriticalSection()
	{
		DeleteCriticalSection(&_cs);
	}

	void CriticalSection::Lock()
	{
		EnterCriticalSection(&_cs);
	}

	void CriticalSection::Unlock()
	{
		LeaveCriticalSection(&_cs);
	}

	bool CriticalSection::TryLock()
	{
		return TryEnterCriticalSection(&_cs) != FALSE;
	}

	//-------------------------------------------------------------------------------

} // namespace sb


