// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "System.h"

///
/// @file System_macosx.cpp
///	@brief Some platform dependent functions for the system
///


namespace sb
{

namespace system
{
	CriticalSection g_stack_walk_lock;

	void SymInitialize();
};
void system::SymInitialize()
{

}

uint32_t system::GetStackTrace(void** addresses, uint32_t max_addresses, uint32_t skip_count)
{
	return 0;
}
void system::GetAddressSymbol(String& symbol, void* address)
{
	symbol = "(undefined)";
}


//-------------------------------------------------------------------------------
void system::GetSystemInfo(SystemInfo& info)
{
	info.num_processors = sysconf( _SC_NPROCESSORS_ONLN );
	info.page_size = getpagesize();
}
//-------------------------------------------------------------------------------

} // namespace sb

