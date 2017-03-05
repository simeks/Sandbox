// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_STREAM_H__
#define __FOUNDATION_STREAM_H__


namespace sb
{

	class Stream : NonCopyable
	{
	public:

		virtual ~Stream() {}

		/// @brief Reads data from the stream
		///	@param dst Pointer to the destination buffer
		///	@param size Number of bytes to read
		virtual size_t Read(void* dst, size_t size) = 0;

		/// @brief Writes data to the stream
		///	@param src Pointer to the source buffer
		///	@param size Number of bytes to write
		virtual size_t Write(const void* src, size_t size) = 0;


		/// @brief Sets the position in the buffer
		/// @return New position in stream if successful, -1 if seek failed
		virtual int64_t Seek(int64_t offset) = 0;

		/// @brief Returns the current position in the buffer
		virtual int64_t Tell() const = 0;

		/// @brief Returns the size of the stream
		virtual int64_t Length() const = 0;


	};

} // namespace sb



#endif // __FOUNDATION_STREAM_H__