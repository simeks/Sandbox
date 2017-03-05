// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "FileInputBuffer.h"
#include "Filesystem/File.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	FileInputBuffer::FileInputBuffer(FileStreamPtr file)
	{
		// Read the entire file to the buffer
		_length = file->Length();
		if (_length > 0)
		{
			_buffer = (char*)memory::Malloc((size_t)_length);
			file->Read(_buffer, (uint32_t)_length);
		}
	}
	FileInputBuffer::~FileInputBuffer()
	{
		if (_buffer)
		{
			memory::Free(_buffer);
		}
		_buffer = nullptr;
		_length = 0;
	}
	//-------------------------------------------------------------------------------

} // namespace sb


