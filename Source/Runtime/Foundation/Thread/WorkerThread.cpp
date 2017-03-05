// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "TaskScheduler.h"
#include "Profiler/Profiler.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	void TaskScheduler::WorkerThread::Start()
	{
		// Start a simple thread with this as a Runnable
		_thread.Start(this);
	}

	// Runnable 
	void TaskScheduler::WorkerThread::Run()
	{
		char name[100];
		snprintf(name, 99, "Worker_%d", GetWorkerIndex());
		MicroProfileOnThreadCreate(&name[0]);

		// Run
		while (1)
		{
			Idle();

			Task* task;
			while (_owner->PopTask(&task))
			{
				// Run task
				_owner->RunTask(task);
			}

			if (_owner->IsShuttingDown())
				break;
		}
	}
	void TaskScheduler::WorkerThread::Idle()
	{
		// Notify that we are going to sleep
		_owner->_sem_thread_sleep.Set();

		// Sleep
		_owner->_sem_thread_wakeup.Wait();
	}

	int TaskScheduler::WorkerThread::GetWorkerIndex()
	{
		return int(this - _owner->_workers[0]);
	}
	//------------------------------------------------------------------------------

} // namespace sb
