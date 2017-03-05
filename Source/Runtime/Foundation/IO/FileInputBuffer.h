// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_FILEINPUTBUFFER_H__
#define __FOUNDATION_FILEINPUTBUFFER_H__

#include "InputBuffer.h"

#include <Foundation/Filesystem/FileStream.h>


namespace sb
{

	/// InputBuffer for files, reads the entire file to memory on construct.
	/// @remark Beware, as it reads the entire file it can be very slow for big files
	/// TODO: Make it more friendly for big files by reading chunks from the file on demand
	class FileInputBuffer : public InputBuffer
	{
	public:
		FileInputBuffer(FileStreamPtr file);
		~FileInputBuffer();

	};

} // namespace sb


#endif // __FOUNDATION_FILEINPUTBUFFER_H__

