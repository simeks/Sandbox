// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_NONCOPYABLE_H__
#define __FOUNDATION_NONCOPYABLE_H__

namespace sb
{

	// Non copyable base class
	class NonCopyable
	{
	public:
		NonCopyable() {}

	private:
		NonCopyable(const NonCopyable&);
		NonCopyable& operator=(const NonCopyable&);
	};

} // namespace sb


#endif // __FOUNDATION_NONCOPYABLE_H__