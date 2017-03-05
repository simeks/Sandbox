// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_LOGGING_H__
#define __FOUNDATION_LOGGING_H__

#include "Log.h"


namespace sb
{

	namespace logging
	{
		/// @brief Initializes the logging system
		///	@param file_name Specifies name for the log file
		void Initialize(const char* file_name);
		void Shutdown();

		///	@param file_name Specifies name for the log file
		void SetOutputFile(const char* file_name);

		void Info(const char* fmt, ...);
		void InfoV(const char* fmt, va_list arg);

		void Warning(const char* fmt, ...);
		void WarningV(const char* fmt, va_list arg);

		void Error(const char* fmt, ...);
		void ErrorV(const char* fmt, va_list arg);

		void AssertMessage(const char* fmt, ...);

		typedef void(*LogCallback)(void*, Log::LogLevel, const char* msg);
		void SetCallback(LogCallback callback, void* data);

		//-------------------------------------------------------------------------------

	} // namespace logging

} // namespace sb




#endif // __FOUNDATION_LOGGING_H__