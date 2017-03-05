// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "FilePath.h"
#include "FileUtil.h"

namespace sb
{

	FilePath::FilePath()
	{
	}
	FilePath::FilePath(const char* path) : _path(path)
	{
	}
	FilePath::FilePath(const string& path) : _path(path)
	{
	}
	FilePath::~FilePath()
	{
	}

	void FilePath::Set(const char* path)
	{
		_path = path;
	}

	void FilePath::Set(const string& path)
	{
		_path = path;
	}
	void FilePath::Clear()
	{
		_path.clear();
	}
	void FilePath::SetSeparator(char c)
	{
		char* path = &_path[0];
		while (*path)
		{
			if (*path == '\\' || *path == '/')
			{
				*path = c;
			}
			path++;
		}
	}
	void FilePath::TrimExtension()
	{
		size_t pos = _path.rfind('.');
		if (pos != string::npos)
		{
			_path.erase(pos);
		}
	}

	string FilePath::Directory() const
	{
		size_t pos = _path.rfind('\\');
		if (pos == string::npos) // Separator may be either '/' or '\\'
			pos = _path.rfind('/');
		if (pos != string::npos)
		{
			return _path.substr(0, pos + 1);
		}
		return "";
	}
	string FilePath::Filename() const
	{
		size_t pos = _path.rfind('\\');
		if (pos == string::npos) // Separator may be either '/' or '\\'
			pos = _path.rfind('/');
		if (pos != string::npos)
		{
			return _path.substr(pos + 1);
		}
		return _path;
	}
	string FilePath::Extension() const
	{
		size_t pos = _path.rfind('.');
		if (pos != string::npos)
		{
			return _path.substr(pos + 1);
		}
		return "";
	}
	string& FilePath::Get()
	{
		return _path;
	}
	const string& FilePath::Get() const
	{
		return _path;
	}
	const char* FilePath::c_str() const
	{
		return _path.c_str();
	}

	FilePath& FilePath::operator+=(const string& other)
	{
		_path += other;
		return *this;
	}
	FilePath& FilePath::operator+=(const char* other)
	{
		_path += other;
		return *this;
	}
	FilePath& FilePath::operator+=(const char other)
	{
		_path += other;
		return *this;
	}

} // namespace sb

