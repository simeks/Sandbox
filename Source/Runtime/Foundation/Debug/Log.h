// Copyright 2008-2014 Simon Ekström

#ifndef __LOG_H__
#define __LOG_H__

#define MAX_MSG_SIZE 4096

namespace sb
{

	/// @brief Representation of a log file
	class Log
	{
		char	_file_name[MAX_PATH];
		FILE*	_file_handle;

	public:
		enum LogLevel
		{
			LOG_INFO = 0,
			LOG_WARNING = 1,
			LOG_ERROR = 2
		};

		Log();
		~Log();

		void Open(const char* file_name);
		void Close();

		bool IsOpen() const;

		//-------------------------------------------------------------------------------
		void Write(const LogLevel level, const char* fmt, ...);
		void WriteV(const LogLevel level, const char* fmt, va_list arg);
		//-------------------------------------------------------------------------------

	};

} // namespace sb


#endif // __LOG_H__

