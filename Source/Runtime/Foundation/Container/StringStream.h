// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_STRINGSTREAM_H__
#define __FOUNDATION_STRINGSTREAM_H__

namespace sb
{

	class StringStream
	{
	public:
		StringStream(Allocator& allocator = memory::DefaultAllocator());
		~StringStream();

		StringStream& operator<<(int val);
		StringStream& operator<<(uint32_t val);
		StringStream& operator<<(int64_t val);
		StringStream& operator<<(uint64_t val);
		StringStream& operator<<(float val);
		StringStream& operator<<(double val);
		StringStream& operator<<(const char* val);
		StringStream& operator<<(char val);
		StringStream& operator<<(bool val);

		void Clear();
		size_t Size() const;


		const String& Str() const;
		const char* Ptr() const;

	private:
		String _buffer;
	};

} // namespace sb

#endif // __FOUNDATION_STRINGSTREAM_H__
