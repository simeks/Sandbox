// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_DIRECTORYWATCHER_H__
#define __FOUNDATION_DIRECTORYWATCHER_H__

#include <Foundation/Thread/Thread.h>

namespace sb
{

#ifdef SANDBOX_PLATFORM_WIN

	/// Struct representing a change in a directory
	struct DirectoryChange
	{
		enum Action { FILE_ADDED, FILE_REMOVED, FILE_MODIFIED, UNKNOWN };

		string path;
		Action action;
	};

	/// @brief Utility for monitoring activity in the filesystem.
	class DirectoryWatcher : public Runnable
	{
	public:
		/// @brief Constructor
		/// @param dir Directory to monitor.
		/// @param watch_subtree Specfies if we should monitor the complete subtree of the directory.
		DirectoryWatcher(const char* dir, bool watch_subtree);
		~DirectoryWatcher();

		/// Pops a change from the change queue
		/// @return True if any change was popped from queue, else false
		bool PopChange(DirectoryChange& change);

	private:
		enum { BUFFER_SIZE = 1024 };

		struct DirectoryInfo
		{
			void* handle; // Directory handle
			char buffer[BUFFER_SIZE];
			bool watch_subtree;
#ifdef _WIN32
			OVERLAPPED overlapped;
#endif	
		};

	private:
		DirectoryInfo _dir_info;
		void* _comp_port_handle; // Handle for completion port

		CriticalSection _queue_lock;
		deque<DirectoryChange> _changes;

		SimpleThread _thread;

	private:
		// Runnable
		void Run();

		void Push(DirectoryChange::Action action, const char* filename);
	};

#endif // SANDBOX_PLATFORM_WIN

} // namespace sb



#endif // __FOUNDATION_DIRECTORYWATCHER_H__

