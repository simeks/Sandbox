// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "../File.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	File::File() : _handle(INVALID_HANDLE_VALUE)
	{
	}
	File::~File()
	{
	}
	bool File::Open(const char* file, File::FileMode file_mode)
	{
		DWORD desired_access = 0;
		DWORD share_mode = 0; // Allow multiple handles for reading if we're not writing to the file
		DWORD creation_disposition = OPEN_EXISTING;

		if (file_mode == File::READ)
		{
			desired_access |= GENERIC_READ;
			share_mode |= FILE_SHARE_READ;
		}
		else if (file_mode == File::WRITE)
		{
			desired_access |= GENERIC_WRITE;
			creation_disposition = CREATE_ALWAYS;
		}
		else if (file_mode == File::APPEND)
		{
			desired_access |= GENERIC_WRITE;
			creation_disposition = OPEN_ALWAYS;
		}

		_handle = ::CreateFile(file, desired_access, share_mode, NULL, creation_disposition, FILE_ATTRIBUTE_NORMAL, NULL);
		if ((_handle != INVALID_HANDLE_VALUE) && (file_mode == File::APPEND))
		{
			Seek(0, SEEK_ORIGIN_END);
		}
		return _handle != INVALID_HANDLE_VALUE;
	}
	void File::Close()
	{
		if (_handle == INVALID_HANDLE_VALUE)
			return;

		Verify(::CloseHandle(_handle) == 1);
		_handle = INVALID_HANDLE_VALUE;
	}
	bool File::IsOpen() const
	{
		return _handle != INVALID_HANDLE_VALUE;
	}
	uint32_t File::Read(void* dst, uint32_t size)
	{
		Assert(_handle != INVALID_HANDLE_VALUE);
		Assert(dst);

		DWORD bytes_read = 0;
		BOOL result = ::ReadFile(_handle, dst, size, &bytes_read, NULL);

		return (result ? bytes_read : 0);
	}
	uint32_t File::Write(const void* src, uint32_t size)
	{
		Assert(_handle != INVALID_HANDLE_VALUE);
		Assert(src);

		DWORD bytes_written = 0;
		BOOL result = ::WriteFile(_handle, src, size, &bytes_written, NULL);

		return (result ? bytes_written : 0);
	}

	int64_t File::Seek(int64_t offset, SeekOrigin origin)
	{
		Assert(_handle != INVALID_HANDLE_VALUE);

		LARGE_INTEGER distance_to_move, new_file_pointer;
		DWORD move_method;
		switch (origin)
		{
		case SEEK_ORIGIN_END:
			move_method = FILE_END;
			break;
		case SEEK_ORIGIN_BEGIN:
			move_method = FILE_BEGIN;
			break;
		default: // SEEK_ORIGIN_CURRENT:
			move_method = FILE_CURRENT;
			break;
		};

		distance_to_move.QuadPart = offset;
		new_file_pointer.QuadPart = 0;

		BOOL result = ::SetFilePointerEx(_handle, distance_to_move, &new_file_pointer, move_method);
		Assert(result);

		return (result ? new_file_pointer.QuadPart : -1);
	}
	void File::Flush()
	{
		Assert(_handle != INVALID_HANDLE_VALUE);
		::FlushFileBuffers(_handle);
	}

	int64_t File::Tell() const
	{
		Assert(_handle != INVALID_HANDLE_VALUE);
		LARGE_INTEGER distance_to_move, new_file_pointer;
		distance_to_move.QuadPart = 0;
		new_file_pointer.QuadPart = 0;

		BOOL result = ::SetFilePointerEx(_handle, distance_to_move, &new_file_pointer, FILE_CURRENT);
		Assert(result);

		return (result ? new_file_pointer.QuadPart : -1);
	}

	int64_t File::Length() const
	{
		LARGE_INTEGER size;
		size.QuadPart = 0;

		BOOL result = ::GetFileSizeEx(_handle, &size);
		Assert(result);

		return (result ? size.QuadPart : -1);
	}

	//-------------------------------------------------------------------------------

} // namespace sb


