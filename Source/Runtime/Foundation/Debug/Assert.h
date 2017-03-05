// Copyright 2008-2014 Simon Ekström

#ifndef _ASSERT_H
#define _ASSERT_H

namespace sb
{
	enum AssertRetval
	{
		RET_DEBUGGER = 0,
		RET_CONTINUE,
		RET_ABORT,
	};

	AssertRetval UtilAssertMsg(const char* msg, const char* file_name, unsigned int line);

} // namespace sb


#if defined(SANDBOX_DEVELOPMENT)
#define Assert( _exp )				if(!(_exp))		\
	{				\
	if (sb::UtilAssertMsg("Assertion Failed: " #_exp, __FILE__, __LINE__) == sb::RET_DEBUGGER) \
	{	\
	DEBUG_BREAK; \
	}	\
	} \
	ANALYSIS_ASSUME(_exp);

#define AssertMsg( _exp, _msg )		if(!(_exp))		\
	{				\
	if (sb::UtilAssertMsg(_msg, __FILE__, __LINE__) == sb::RET_DEBUGGER) \
	{	\
	DEBUG_BREAK; \
	}	\
	} \
	ANALYSIS_ASSUME(_exp);

#define Verify( _exp ) Assert( _exp )

#else

#define Assert( _exp )				((void)0)
#define AssertMsg( _exp, _msg )		((void)0)
#define Verify( _exp)				(_exp)


#endif




#endif // _ASSERT_H