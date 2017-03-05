// Copyright 2008-2014 Simon Ekström

#ifndef __SYSTEM_H__
#define __SYSTEM_H__


namespace sb
{

	/// @brief System utilities
	namespace system
	{
		/// @brief Some general system info
		struct SystemInfo
		{
			uint32_t num_processors;
			uint32_t page_size;
		};

		void GetSystemInfo(SystemInfo& info);

		/// @brief Do a stackwalk and return the current stack trace
		///	@param addresses		An pre-allocated array for storing the addresses from the backtrace
		///	@param max_addresses	Maximum number of addresses to store in our array
		///	@param skip_count		Number of stack levels to skip before we start filling the array
		uint32_t GetStackTrace(void** addresses, uint32_t max_addresses, uint32_t skip_count = 0);

		/// @brief Get the symbol for a specified address
		///	@param symbol	Resulting symbol
		///	@param address	Address to translate
		void GetAddressSymbol(string& symbol, void* address);

	};

} // namespace sb


#endif // __SYSTEM_H__