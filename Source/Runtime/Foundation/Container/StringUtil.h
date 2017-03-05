// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_STRINGUTIL_H__
#define __FOUNDATION_STRINGUTIL_H__

namespace sb
{

	namespace string_util
	{

		/// @brief Formats the specified value and puts it in the given string.
		template<typename T>
		void ToString(char* dest, uint32_t len, T value);

		std::string Format(const char* fmt, ...);

	} // namespace string_util

} // namespace sb


#include "StringUtil.inl"


#endif // __FOUNDATION_STRINGUTIL_H__
