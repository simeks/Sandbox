// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "FileSource.h"
#include "FileSystem.h"
#include "File.h"
#include "FileUtil.h"
#include "FilePath.h"

#include <sys/stat.h>

namespace sb
{

	//-------------------------------------------------------------------------------
	FileTime::FileTime() : low(0), high(0)
	{
	}

	bool FileTime::operator==(const FileTime& other) const
	{
		return ((high == other.high) && (low == other.low));
	}
	bool FileTime::operator!=(const FileTime& other) const
	{
		return ((high != other.high) || (low != other.low));
	}
	bool FileTime::operator<(const FileTime& other) const
	{
		if (high < other.high)
			return true;
		if (high > other.high)
			return false;
		if (low < other.low)
			return true;
		return false;
	}
	bool FileTime::operator>(const FileTime& other) const
	{
		if (high > other.high)
			return true;
		if (high < other.high)
			return false;
		if (low > other.low)
			return true;
		return false;
	}
	bool FileTime::operator<=(const FileTime& other) const
	{
		if (high < other.high)
			return true;
		if (high > other.high)
			return false;
		if (low < other.low)
			return true;
		if (low > other.low)
			return false;

		return true;
	}
	bool FileTime::operator>=(const FileTime& other) const
	{
		if (high > other.high)
			return true;
		if (high < other.high)
			return false;
		if (low > other.low)
			return true;
		if (low < other.low)
			return false;

		return true;
	}



	//-------------------------------------------------------------------------------
	FileSource::FileSource(FileSystem* file_system, const char* path)
		: _file_system(file_system)
	{
		_path = path;
	}
	FileSource::~FileSource()
	{
	}

	FileStreamPtr FileSource::OpenFile(const char* file_path, const File::FileMode mode)
	{
		string full_path;
		const char* _mode;

		switch (mode)
		{
		case File::READ:
			_mode = "rb";
			break;
		case File::WRITE:
			_mode = "wb";
			break;
		case File::APPEND:
			_mode = "ab";
			break;
		}
		// Is the sources file path absolute?
		if (_path.find(':') != string::npos)
		{
			full_path = _path + PATH_SEPARATOR + file_path;
		}
		else
		{
			file_util::BuildOSPath(full_path, _file_system->GetBasePath(), _path);
			full_path += PATH_SEPARATOR;
			full_path += file_path;
		}

		file_util::FixSlashes(full_path);

		File file;
		if (file.Open(full_path.c_str(), mode))
		{
			return FileStreamPtr(new FileStream(file));
		}

		return FileStreamPtr();
	}

	void FileSource::MakeDirectory(const char* path)
	{
		string full_path;
		// Is the sources file path absolute?
		if (_path.find(':') != string::npos)
		{
			full_path = _path.c_str();
			full_path += PATH_SEPARATOR;
			full_path += path;
		}
		else
		{
			file_util::BuildOSPath(full_path, _file_system->GetBasePath(), _path);
			full_path += PATH_SEPARATOR;
			full_path += path;
		}
		full_path += PATH_SEPARATOR; // Add trailing separator

		file_util::FixSlashes(full_path);

		size_t end = 0;


#ifdef SANDBOX_PLATFORM_WIN
		struct _stati64 stat_info;
		while ((end = full_path.find(PATH_SEPARATOR, end + 1)) != string::npos)
		{
			string current_path = full_path.substr(0, end);
			if ((current_path.rfind(':', 0) != (current_path.size() - 1)) // Avoid trying to create a device (E.g. "C:")
				|| (_stat64(current_path.c_str(), &stat_info) != 0))
			{
				CreateDirectory(current_path.c_str(), NULL);
			}
		}
#elif SANDBOX_PLATFORM_MACOSX
		struct stat stat_info;
		while((end = full_path.Find(PATH_SEPARATOR, end+1)) != String::npos)
		{
			stringcurrent_path = full_path.Substr(0, end);
			if((current_path.RFind(':', 0) != (current_path.Size()-1)) // Avoid trying to create a device (E.g. "C:")
				|| (stat(current_path.Ptr(), &stat_info) != 0))
			{
				mkdir(current_path.Ptr(), S_IRWXU);
			}
		}
#endif
	}


	void FileSource::FindFiles(const char* path, vector<string>& files) const
	{
		string full_path;

		// Is the sources file path absolute?
		if (_path.find(':') != string::npos)
		{
			full_path = _path + PATH_SEPARATOR + path;
		}
		else
		{
			file_util::BuildOSPath(full_path, _file_system->GetBasePath(), _path);
			full_path += PATH_SEPARATOR;
			full_path += path;
		}

		file_util::FindFiles(full_path.c_str(), files);
	}
	void FileSource::FindDirectories(const char* path, vector<string>& directories) const
	{
		string full_path;

		// Is the sources file path absolute?
		if (_path.find(':') != string::npos)
		{
			full_path = _path + PATH_SEPARATOR + path;
		}
		else
		{
			file_util::BuildOSPath(full_path, _file_system->GetBasePath(), _path);
			full_path += PATH_SEPARATOR;
			full_path += path;
		}

		file_util::FindDirectories(full_path.c_str(), directories);
	}
	bool FileSource::LastModifiedTime(const char* file_path, FileTime& file_time) const
	{
		Assert(file_path);

		string full_path;

		// Is the sources file path absolute?
		if (_path.find(':') != string::npos)
		{
			full_path = _path + PATH_SEPARATOR + file_path;
		}
		else
		{
			file_util::BuildOSPath(full_path, _file_system->GetBasePath(), _path);
			full_path += PATH_SEPARATOR;
			full_path += file_path;
		}

#ifdef SANDBOX_PLATFORM_WIN
		WIN32_FILE_ATTRIBUTE_DATA file_attr;

		memset(&file_attr, 0, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
		if (GetFileAttributesEx(full_path.c_str(), GetFileExInfoStandard, &file_attr))
		{
			file_time.high = file_attr.ftLastWriteTime.dwHighDateTime;
			file_time.low = file_attr.ftLastWriteTime.dwLowDateTime;

			return true;
		}

#elif SANDBOX_PLATFORM_MACOSX
		struct stat stat_info;
		if(stat(full_path.Ptr(), &stat_info) == 0)
		{
			uint64_t t = stat_info.st_mtime;
			file_time.high = (uint32_t)(t >> 32);
			file_time.low = (uint32_t)(t & 0xFFFFFFFF);

			return true;
		}

#endif

		return false;
	}
	const string& FileSource::GetPath() const
	{
		return _path;
	}
	string FileSource::GetFullPath() const
	{
		// Is the sources file path absolute?
		if (_path.find(':') != string::npos)
		{
			return _path;
		}

		string full_path;
		file_util::BuildOSPath(full_path, _file_system->GetBasePath(), _path);
		
		return full_path;
	}
	//-------------------------------------------------------------------------------

} // namespace sb

