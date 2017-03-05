// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "StringUtil.h"

namespace sb
{

	std::string string_util::Format(const char* fmt, ...)
	{
		char buf[2048];

		va_list vl;
		va_start(vl, fmt);
		vsnprintf(buf, 2048, fmt, vl);
		va_end(vl);

		buf[2047] = '\0';
		return std::string(buf);
	}

} // namespace sb
