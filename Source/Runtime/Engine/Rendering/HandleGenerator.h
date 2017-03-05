// Copyright 2008-2014 Simon Ekström

#ifndef __ENGINE_HANDLEGENERATOR_H__
#define __ENGINE_HANDLEGENERATOR_H__

#include <Foundation/Thread/Lock.h>

namespace sb
{

	/// Thread-safe generator used for generating unique handles for resources.
	class HandleGenerator
	{
	public:
		HandleGenerator();
		~HandleGenerator();

		/// @brief Generates a new resource handle
		uint32_t New();

		/// @brief Releases the given handle
		void Release(uint32_t h);

	private:
		CriticalSection _lock;

		vector<uint32_t> _free_handles;
		uint32_t _current_handle;

	};

} // namespace sb


#endif // __ENGINE_HANDLEGENERATOR_H__
