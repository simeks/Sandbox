// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_INPUTBUFFER_H__
#define __FOUNDATION_INPUTBUFFER_H__


namespace sb
{

	class InputBuffer
	{
	public:
		InputBuffer();
		virtual ~InputBuffer();

		void* Ptr();
		int64_t Length() const;

	protected:
		char*	_buffer;
		int64_t _length;

	};

} // namespace sb


#endif // __FOUNDATION_INPUTBUFFER_H__

