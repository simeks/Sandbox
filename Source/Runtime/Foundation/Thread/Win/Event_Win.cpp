// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "../Event.h"

#include <process.h>


namespace sb
{

	//-------------------------------------------------------------------------------
	Event::Event(bool manual_reset, bool initial_state)
	{
		_handle = (void*)CreateEvent(NULL, manual_reset, initial_state, NULL);
	}
	Event::~Event()
	{
		CloseHandle(_handle);
	}

	void Event::Set()
	{
		SetEvent(_handle);
	}

	void Event::Reset()
	{
		ResetEvent(_handle);
	}

	void Event::Wait()
	{
		WaitForSingleObject(_handle, INFINITE);
	}

	bool Event::Wait(uint32_t milliseconds)
	{
		if (WaitForSingleObject(_handle, milliseconds) == WAIT_TIMEOUT)
			return false;
		return true;
	}
	//-------------------------------------------------------------------------------

} // namespace sb


