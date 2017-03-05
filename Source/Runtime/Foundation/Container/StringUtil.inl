// Copyright 2008-2014 Simon Ekström

namespace sb
{

	namespace string_util
	{

#define IMPLEMENT_TO_STRING(type, fmt) \
	INLINE void ToStringImpl(char* dst, uint32_t len, type value) \
		{ \
		snprintf(dst, len, fmt, value); \
		dst[len-1] = '\0'; \
	}

		IMPLEMENT_TO_STRING(int32_t, "%d");
		IMPLEMENT_TO_STRING(uint32_t, "%u");
		IMPLEMENT_TO_STRING(int64_t, "%lld");
		IMPLEMENT_TO_STRING(uint64_t, "%llu");
		IMPLEMENT_TO_STRING(const void*, "%p");
		IMPLEMENT_TO_STRING(double, "%f");

#undef IMPLEMENT_TO_STRING

		template<typename T>
		void ToString(char* dest, uint32_t len, T value)
		{
			ToStringImpl(dest, len, value);
		}



	} // namespace string_util

} // namespace sb
