// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_FILE_H__
#define __FOUNDATION_FILE_H__

namespace sb
{

	class File
	{
	public:
		enum SeekOrigin
		{
			SEEK_ORIGIN_CURRENT,
			SEEK_ORIGIN_END,
			SEEK_ORIGIN_BEGIN
		};

		enum FileMode
		{
			READ = (1 << 0),
			WRITE = (1 << 1),
			APPEND = (1 << 2)
		};

		File();
		~File();

		bool Open(const char* file, File::FileMode file_mode);
		void Close();
		bool IsOpen() const;

		void Flush();

		/// @brief Reads data from the file
		///	@param dst Pointer to the destination buffer
		///	@param size Number of bytes to read
		uint32_t Read(void* dst, uint32_t size);

		/// @brief Writes data to the file
		///	@param src Pointer to the source buffer
		///	@param size Number of bytes to write
		uint32_t Write(const void* src, uint32_t size);


		/// @brief Sets the position in the file
		/// @return New position in stream if successful, -1 if seek failed
		int64_t Seek(int64_t offset, SeekOrigin origin = SEEK_ORIGIN_BEGIN);

		/// @brief Returns the current position in the file
		int64_t Tell() const;

		/// @brief Returns the size of the file
		int64_t Length() const;

	private:
#ifdef SANDBOX_PLATFORM_WIN
		typedef HANDLE Handle;
#elif SANDBOX_PLATFORM_MACOSX
		typedef int Handle;
#endif

		Handle _handle;

	};

} // namespace sb



#endif // __FOUNDATION_FILE_H__
