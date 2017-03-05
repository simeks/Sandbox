// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "HandleGenerator.h"

namespace sb
{

	HandleGenerator::HandleGenerator()
		: _current_handle(0)
	{
	}
	HandleGenerator::~HandleGenerator()
	{
		Assert(_current_handle == _free_handles.size()); // Check if all allocated handles are released
	}
	uint32_t HandleGenerator::New()
	{
		ScopedLock<CriticalSection> scoped_lock(_lock);

		if (!_free_handles.empty())
		{
			uint32_t h = _free_handles.back();
			_free_handles.pop_back();
			return h;
		}

		uint32_t handle = _current_handle++;
		Assert(IsValid(handle)); // Out of handles.

		return handle;
	}
	void HandleGenerator::Release(uint32_t h)
	{
		ScopedLock<CriticalSection> scoped_lock(_lock);
		_free_handles.push_back(h);
	}

} // namespace sb
