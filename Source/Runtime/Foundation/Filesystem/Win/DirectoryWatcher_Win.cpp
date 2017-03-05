// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "../DirectoryWatcher.h"


namespace sb
{

#ifdef SANDBOX_PLATFORM_WIN

	DirectoryWatcher::DirectoryWatcher(const char* dir, bool watch_subtree)
		: _comp_port_handle(NULL)
	{
		memset(&_dir_info, 0, sizeof(DirectoryInfo));

		_dir_info.watch_subtree = watch_subtree;
		_dir_info.handle = CreateFile(dir, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
		Assert(_dir_info.handle != INVALID_HANDLE_VALUE);

		if (_dir_info.handle == INVALID_HANDLE_VALUE)
		{
			return;
		}

		_comp_port_handle = CreateIoCompletionPort(_dir_info.handle, _comp_port_handle, (ULONG_PTR)&_dir_info, 0);
		Assert(_comp_port_handle != INVALID_HANDLE_VALUE);

		_thread.Start(this);
	}
	DirectoryWatcher::~DirectoryWatcher()
	{
		// Signal thread to exit
		PostQueuedCompletionStatus(_comp_port_handle, 0, 0, NULL);

		// Wait until it exits
		_thread.Join();

		CloseHandle(_comp_port_handle);
		CloseHandle(_dir_info.handle);

		_changes.clear();
	}

	bool DirectoryWatcher::PopChange(DirectoryChange& change)
	{
		ScopedLock<CriticalSection> scoped_lock(_queue_lock);
		if (!_changes.empty())
		{
			change = _changes.front();
			_changes.pop_front();

			return true;
		}
		return false;
	}
	void DirectoryWatcher::Run()
	{
		Verify(ReadDirectoryChangesW(
			(HANDLE)_dir_info.handle,
			(void*)_dir_info.buffer,
			BUFFER_SIZE,
			(BOOL)_dir_info.watch_subtree,
			FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME,
			NULL,
			&_dir_info.overlapped,
			NULL) != 0);

		DWORD num_bytes;
		DWORD offset;
		LPOVERLAPPED overlapped;
		PFILE_NOTIFY_INFORMATION notify_info;
		DirectoryInfo* dir_info = 0;

		DirectoryChange::Action action = DirectoryChange::UNKNOWN;
		char file_name[MAX_PATH] = "";

		do
		{
			GetQueuedCompletionStatus(_comp_port_handle, &num_bytes, (PULONG_PTR)&dir_info, &overlapped, INFINITE);
			if (dir_info)
			{
				notify_info = (PFILE_NOTIFY_INFORMATION)dir_info->buffer;
				do
				{
					offset = notify_info->NextEntryOffset;
					switch (notify_info->Action)
					{
					case FILE_ACTION_ADDED:
						action = DirectoryChange::FILE_ADDED;
						break;
					case FILE_ACTION_REMOVED:
						action = DirectoryChange::FILE_REMOVED;
						break;
					case FILE_ACTION_MODIFIED:
						action = DirectoryChange::FILE_MODIFIED;
						break;
					case FILE_ACTION_RENAMED_OLD_NAME:
						action = DirectoryChange::FILE_REMOVED;
						break;
					case FILE_ACTION_RENAMED_NEW_NAME:
						action = DirectoryChange::FILE_ADDED;
						break;
					};
					wcstombs(file_name, notify_info->FileName, MAX_PATH);
					file_name[notify_info->FileNameLength / sizeof(WCHAR)] = '\0';

					Push(action, file_name);

					notify_info = (PFILE_NOTIFY_INFORMATION)((LPBYTE)notify_info + offset);
				} while (offset != 0);

				ReadDirectoryChangesW(
					_dir_info.handle,
					_dir_info.buffer,
					BUFFER_SIZE,
					_dir_info.watch_subtree,
					FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME,
					NULL,
					&_dir_info.overlapped,
					NULL);
			}

		} while (dir_info);


	}
	void DirectoryWatcher::Push(DirectoryChange::Action action, const char* file_name)
	{
		ScopedLock<CriticalSection> scoped_lock(_queue_lock);

		_changes.push_back(DirectoryChange());
		DirectoryChange& change = _changes.back();
		change.action = action;
		change.path = file_name;

	}

#endif // SANDBOX_PLATFORM_WIN

} // namespace sb
