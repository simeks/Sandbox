// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "FileSystem.h"
#include "FileSource.h"
#include "File.h"
#include "FileUtil.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	FileSystem::FileSystem(const char* base_path)
	{
		_base_path = base_path;
	}
	FileSystem::~FileSystem()
	{
		// Clear any file sources that are left
		for (auto& source : _file_sources)
		{
			delete source;
		}
		_file_sources.clear();
	}
	FileStreamPtr FileSystem::OpenFile(const char* file_path, const File::FileMode mode)
	{
		Assert(!_file_sources.empty());
		FileStreamPtr file;
		for (auto& source : _file_sources)
		{
			file = source->OpenFile(file_path, mode);
			if (file.Get())
			{
				return file;
			}
		}
		logging::Warning("FileSystem: File '%s' not found", file_path);
		return FileStreamPtr();
	}
	FileSource* FileSystem::OpenFileSource(const char* path, const PathAddFlags flags)
	{
		FileSource* source = 0;

		// First check if the path already exists in the system
		for (auto& source : _file_sources)
		{
			if (source->GetPath() == path)
			{
				return source;
			}
		}

		source = new FileSource(this, path);

		if (flags == ADD_TO_TAIL)
		{
			_file_sources.push_back(source);
		}
		else
		{
			_file_sources.push_front(source);
		}

		return source;
	}
	void FileSystem::CloseFileSource(FileSource* source)
	{
		deque<FileSource*>::iterator it = std::find(_file_sources.begin(), _file_sources.end(), source);
		if (it != _file_sources.end())
		{
			delete source;
			_file_sources.erase(it);
			return;
		}

	}
	void FileSystem::CloseFileSource(const char* path)
	{
		for (deque<FileSource*>::iterator it = _file_sources.begin(); it != _file_sources.end(); ++it)
		{
			if ((*it)->GetPath() == path)
			{
				delete (*it);
				_file_sources.erase(it);
				return;
			}
		}
	}
	void FileSystem::MakeDirectory(const char* path)
	{
		string dir_path(path);
		string full_path;
		// Is the sources file path absolute?
		if (dir_path.find(':') != string::npos)
		{
			full_path = dir_path;
		}
		else
		{
			file_util::BuildOSPath(full_path, _base_path, dir_path);
		}

#ifdef SANDBOX_PLATFORM_WIN
		CreateDirectory(full_path.c_str(), NULL);
#elif SANDBOX_PLATFORM_MACOSX
		mkdir(full_path.Ptr(), S_IRWXU);
#endif
	}

	const string& FileSystem::GetBasePath() const
	{
		return _base_path;
	}
	//-------------------------------------------------------------------------------

} // namespace sb

