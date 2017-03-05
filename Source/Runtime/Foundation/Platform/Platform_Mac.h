// Copyright 2008-2014 Simon Ekström

#ifndef _PLATFORM_MACOSX_H
#define _PLATFORM_MACOSX_H

// Some macros and defines 

#define DEBUG_BREAK __builtin_trap()
#define ANALYSIS_ASSUME(expr)

#define INLINE inline __attribute__( ( always_inline ))


#if defined(__GNUC__) && !defined(_RELEASE)
	#define ATTR_PRINTF(...) __attribute__ ((format(printf, __VA_ARGS__)))
#else
	#define ATTR_PRINTF(...)  
#endif

// Max length of path name, from windows
#define MAX_PATH 260


// Endianness
#define PLATFORM_LITTLE_ENDIAN

#ifdef __BIG_ENDIAN__
#error Not supported
#endif 



#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// type traits
#include <type_traits>

namespace std
{
	template< class T > struct has_trivial_assign : is_trivially_assignable< T, T > {};
	template< class T > struct has_trivial_constructor : is_trivially_constructible< T > {};
	template< class T > struct has_trivial_destructor : is_trivially_destructible< T > {};
	template< class T > struct has_trivial_copy : is_trivially_copyable< T > {};
}

// Types

// Some typedefs from windows, try avoid using these
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;


// String utilities for cross platform

template<size_t SIZE>
int strcpy_s(char (&dst)[SIZE], const char* src)
{
	strncpy(dst, src, SIZE);
	dst[SIZE-1] = '\0';
	return 0;
}

template<size_t SIZE>
int strcat_s(char (&dst)[SIZE], const char* src)
{
	strncat(dst, src, SIZE);
	dst[SIZE-1] = '\0';
	return 0;
}

INLINE int strcpy_s(char* dst, size_t size, const char* src)
{
	strncpy(dst, src, size);
	dst[size-1] = '\0';
	return 0;
}

INLINE void Sleep(int milliseconds)
{
	usleep(milliseconds * 1000);
}

#endif // _PLATFORM_MACOSX_H
