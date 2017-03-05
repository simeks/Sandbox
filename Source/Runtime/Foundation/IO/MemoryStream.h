// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_MEMORYSTREAM_H__
#define __FOUNDATION_MEMORYSTREAM_H__

#include "Stream.h"


namespace sb
{

	/// @brief Static memory stream for reading from a preallocated buffer
	class StaticMemoryStream : public Stream
	{
		uint8_t* _start;
		uint8_t* _current;
		uint8_t* _end;

	public:
		StaticMemoryStream(void* data, size_t size);
		~StaticMemoryStream();

		size_t Read(void* dst, size_t size);
		size_t Write(const void* src, size_t size);

		/// Returns a pointer to the current location of the stream
		uint8_t* Current();


		int64_t Seek(int64_t offset);
		int64_t Tell() const;
		bool Eof() const;

		int64_t Length() const;

		bool Readable() const;
		bool Writeable() const;

	};

	/// @brief Stream for reading and writing to a dynamic buffer
	class DynamicMemoryStream : public Stream
	{
		vector<uint8_t>* _buffer;
		size_t _offset;

	public:
		explicit DynamicMemoryStream(vector<uint8_t>* buffer);
		~DynamicMemoryStream();

		size_t Read(void* dst, size_t size);
		size_t Write(const void* src, size_t size);

		int64_t Seek(int64_t offset);
		int64_t Tell() const;
		bool Eof() const;

		int64_t Length() const;

		bool Readable() const;
		bool Writeable() const;

	};

} // namespace sb


#endif // __FOUNDATION_MEMORYSTREAM_H__

