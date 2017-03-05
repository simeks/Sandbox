// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_FILESTREAM_H__
#define __FOUNDATION_FILESTREAM_H__

#include <Foundation/IO/Stream.h>

#include "File.h"

namespace sb
{

	class FileStream : public Stream
	{
	public:
		FileStream(const File& file);
		~FileStream();

		/// @brief Reads data from the stream
		///	@param dst Pointer to the destination buffer
		///	@param size Number of bytes to read
		size_t Read(void* dst, size_t size);

		/// @brief Writes data to the stream
		///	@param src Pointer to the source buffer
		///	@param size Number of bytes to write
		size_t Write(const void* src, size_t size);

		/// @brief Sets the position in the buffer
		/// @return New position in stream if successful, -1 if seek failed
		int64_t Seek(int64_t offset);

		/// @brief Returns the current position in the buffer
		int64_t Tell() const;

		/// @brief Returns the size of the stream
		int64_t Length() const;

	private:
		File _file;

	};

	typedef SharedPtr<FileStream> FileStreamPtr;

} // namespace sb


#endif // __FOUNDATION_FILESTREAM_H__
