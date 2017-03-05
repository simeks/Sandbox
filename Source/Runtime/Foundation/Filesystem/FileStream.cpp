// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "FileStream.h"

namespace sb
{

	FileStream::FileStream(const File& file) : _file(file)
	{
		Assert(_file.IsOpen());
	}
	FileStream::~FileStream()
	{
		_file.Close();
	}

	size_t FileStream::Read(void* dst, size_t size)
	{
		return _file.Read(dst, (uint32_t)size);
	}

	size_t FileStream::Write(const void* src, size_t size)
	{
		return _file.Write(src, (uint32_t)size);
	}

	int64_t FileStream::Seek(int64_t offset)
	{
		return _file.Seek(offset, File::SEEK_ORIGIN_BEGIN);
	}

	int64_t FileStream::Tell() const
	{
		return _file.Tell();
	}

	int64_t FileStream::Length() const
	{
		return _file.Length();
	}

} // namespace sb

