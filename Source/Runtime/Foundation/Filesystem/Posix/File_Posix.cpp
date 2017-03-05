// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "../File.h"


namespace sb
{

//-------------------------------------------------------------------------------
File::File() 
	: _handle(-1)
{
}
File::~File()
{
	Close();
}
bool File::Open(const char* file, File::FileMode file_mode)
{
	_file_mode = file_mode;

	mode_t flags = 0;
	if(file_mode == File::READ)
	{
		flags = O_RDONLY;
	}
	else if(file_mode == File::WRITE)
	{
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	}
	else if(file_mode == File::APPEND)
	{
		flags = O_RDWR | O_CREAT;
	}

	_handle = open(file, flags, S_IRWXU);
    if(_handle == -1)
        logging::Warning("%d", errno);
    
	if((_handle >= 0) && (file_mode == File::APPEND))
	{
		Seek(0, SEEK_ORIGIN_END);
	}
	return _handle >= 0;
}
void File::Close()
{
    if(_handle == -1)
        return;
    
	int res = close(_handle);
	Assert(res == 0);
	_handle = -1;
}
bool File::IsOpen() const
{
	return _handle >= 0;
}
uint32_t File::Read(void* dst, uint32_t size)
{
	Assert(_handle != -1);
	Assert(dst)
	
	int result = read(_handle, dst, size);

	return ((result >= 0) ? (uint32_t)result : 0); 
}
uint32_t File::Write(const void* src, uint32_t size)
{
	Assert(_handle != -1);
	Assert(src)

	int result = write(_handle, src, size);

	return ((result >= 0) ? (uint32_t)result : 0); 
}

int64_t File::Seek(int64_t offset, SeekOrigin origin)
{
	Assert(_handle != -1);
	
	int move_method;
	switch(origin)
	{
	case SEEK_ORIGIN_END:
		move_method = SEEK_END;
		break;
	case SEEK_ORIGIN_BEGIN:
		move_method = SEEK_SET;
		break;
	default: // SEEK_ORIGIN_CURRENT:
		move_method = SEEK_CUR;
		break;
	};

	return lseek(_handle, offset, move_method);
}
void File::Flush()
{
	Assert(_handle != -1);
	int res = fsync(_handle);
	Assert(res == 0);
}

int64_t File::Tell() const
{
	Assert(_handle != -1);
	return lseek(_handle, 0, SEEK_CUR);
}
	
int64_t File::Length() const
{
	struct stat file_stat;
	int res = fstat(_handle, &file_stat);
	return (res == 0 ? file_stat.st_size : - 1);
}

//-------------------------------------------------------------------------------

} // namespace sb


