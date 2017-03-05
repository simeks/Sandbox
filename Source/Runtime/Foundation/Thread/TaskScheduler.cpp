// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "TaskScheduler.h"
#include "Platform/System.h"



namespace sb
{

	//-------------------------------------------------------------------------------
	TaskScheduler::TaskScheduler()
		: _next_task_id(0)
	{
		_num_threads = 0;
		_shutting_down = false;

		system::SystemInfo system_info;
		system::GetSystemInfo(system_info);
		_num_processors = system_info.num_processors;
	}
	TaskScheduler::~TaskScheduler()
	{
		if (!_workers.empty()) // Still initialized
			Shutdown();
	}
	//-------------------------------------------------------------------------------
	void TaskScheduler::Initialize()
	{
		_shutting_down = false;

		if (!_num_threads)
			_num_threads = _num_processors - 1; // -1 for the main thread
		if (_num_threads > MAX_WORKER_THREAD_COUNT)
			_num_threads = MAX_WORKER_THREAD_COUNT;

		_workers.resize(_num_threads);

		for (uint32_t i = 0; i < _num_threads; ++i)
		{
			_workers[i] = new WorkerThread(this);
			_workers[i]->Start();
		}
	}
	void TaskScheduler::Shutdown()
	{
		_shutting_down = true;

		// Wait for workers to finish
		WaitForWorkers();
		// Wake them and let them quit
		WakeWorkers();

		for (uint32_t i = 0; i < _num_threads; ++i)
		{
			while (_workers[i]->IsRunning())
			{
				Sleep(1);
			}
			delete _workers[i];
		}
		_workers.clear();
	}
	//-------------------------------------------------------------------------------
	TaskId TaskScheduler::PrepareTask(const WorkItem& work_item)
	{
		TaskId task_id;

		// Allocate a new task from our memory pool
		Task* task = AllocateTask(task_id);
		task->parent = Invalid<uint32_t>();
		task->num_work_items = 1;
		task->work_item.kernel = work_item.kernel;
		task->work_item.data = work_item.data;

		return task_id;
	}
	TaskId TaskScheduler::PrepareEmptyTask()
	{
		TaskId task_id;

		// Allocate a new task from our memory pool
		Task* task = AllocateTask(task_id);
		task->parent = Invalid<uint32_t>();
		task->num_work_items = 1;
		task->work_item.kernel = 0;
		task->work_item.data = 0;

		return task_id;
	}
	//-------------------------------------------------------------------------------
	void TaskScheduler::SetParent(const TaskId& task_id, const TaskId& parent_id)
	{
		Task* task = _task_pool.GetObject(task_id.pool_handle);
		Assert(task);
		task->parent = parent_id.pool_handle;
		Assert(task_id.pool_handle != parent_id.pool_handle);

		Task* parent = _task_pool.GetObject(parent_id.pool_handle);
		Assert(parent);
		thread::InterlockedIncrement(&parent->num_work_items);
	}
	//-------------------------------------------------------------------------------
	void TaskScheduler::SpawnTask(const TaskId& task_id)
	{
		Task* task = _task_pool.GetObject(task_id.pool_handle);
		Assert(task->id == task_id.id);
		PushTask(task);
	}
	void TaskScheduler::SpawnTaskAndWait(const TaskId& task_id)
	{
		Task* wait_for_task = _task_pool.GetObject(task_id.pool_handle);
		Assert(wait_for_task->id == task_id.id);
		PushTask(wait_for_task);

		while (!HasCompleted(task_id))
		{
			Task* task;
			if (PopTask(&task))
			{
				RunTask(task);
			}
			else
			{
				// No work available, yield and let the other threads work
				Sleep(0);
			}
		}
	}
	//-------------------------------------------------------------------------------
	void TaskScheduler::SetWorkerCount(uint32_t num)
	{
		Assert(num > MAX_WORKER_THREAD_COUNT);
		if (!num)
			num = _num_processors;

		if (num == _num_threads)
			return;

		// Restart the manager
		Shutdown();
		_num_threads = num;
		Initialize();
	}
	uint32_t TaskScheduler::GetWorkerCount() const
	{
		return _num_threads;
	}

	//-------------------------------------------------------------------------------
	TaskScheduler::Task* TaskScheduler::AllocateTask(TaskId& task_id)
	{
		Task* task = _task_pool.Allocate();
		memset(task, 0, sizeof(Task));
		Assert(task);
		task->id = thread::InterlockedIncrement(&_next_task_id);

		task_id.id = task->id;
		task_id.pool_handle = _task_pool.GetIndex(task);
		Assert(IsValid<uint32_t>(task_id.pool_handle));

		return task;
	}
	void TaskScheduler::ReleaseTask(Task* task)
	{
		task->parent = Invalid<uint32_t>();
		task->id = thread::InterlockedIncrement(&_next_task_id);
		_task_pool.Release(task);
	}
	//-------------------------------------------------------------------------------
	void TaskScheduler::PushTask(Task* task)
	{
		bool run_now = false;

		// Run directly if we're single threaded
		if (GetWorkerCount() < 1)
		{
			run_now = true;
		}
		else
			// Push task to the queue 
		{
			ScopedLock<CriticalSection> scoped_lock(_task_lock);
			_task_queue.push_back(task);
		}

		WakeWorkers();

		// Run the task now if it wasn't added
		if (run_now)
		{
			RunTask(task);
		}
	}
	//-------------------------------------------------------------------------------
	void TaskScheduler::RunTask(Task* task)
	{
		if (task->work_item.kernel)
			task->work_item.kernel(task->work_item.data);

		CompleteTask(task);
	}
	//-------------------------------------------------------------------------------
	void TaskScheduler::CompleteTask(Task* task)
	{
		// The task is now complete so we decrement our number of workitems
		if (thread::InterlockedDecrement(&task->num_work_items) == 0)
		{
			// Also notify the tasks parent
			if (IsValid(task->parent))
			{
				Task* parent = _task_pool.GetObject(task->parent);
				Assert(parent);
				CompleteTask(parent);
			}
			ReleaseTask(task);
		}
	}
	//-------------------------------------------------------------------------------
	bool TaskScheduler::HasCompleted(const TaskId& task_id)
	{
		Task* task = _task_pool.GetObject(task_id.pool_handle);
		Assert(task);
		if (task->id != task_id.id)
		{
			return true;
		}
		else if (task->num_work_items == 0)
		{
			return true;
		}

		return false;
	}
	//-------------------------------------------------------------------------------
	bool TaskScheduler::PopTask(Task** task)
	{
		ScopedLock<CriticalSection> scoped_lock(_task_lock);

		if (_task_queue.empty())
			return false;

		*task = _task_queue.back();
		_task_queue.pop_back();

		return true;
	}
	//-------------------------------------------------------------------------------
	void TaskScheduler::WakeWorkers()
	{
		for (uint32_t i = 0; i < _num_threads; ++i)
			_sem_thread_wakeup.Set();
	}
	//-------------------------------------------------------------------------------
	void TaskScheduler::WaitForWorkers()
	{
		// Wait for all workers to go to sleep
		for (uint32_t i = 0; i < _num_threads; ++i)
			_sem_thread_sleep.Wait();
	}
	//-------------------------------------------------------------------------------
	bool TaskScheduler::IsShuttingDown() const
	{
		return _shutting_down;
	}

	//------------------------------------------------------------------------------

	namespace parallel_for_internal
	{
		struct ParallelForTaskData
		{
			ParallelForKernel kernel;
			Range range;
			void* data;
		};

		void ParallelForTaskKernel(void* data)
		{
			ParallelForTaskData* parallelfor_data = (ParallelForTaskData*)data;
			parallelfor_data->kernel(parallelfor_data->data, parallelfor_data->range);
		}
	}
	void scheduling::ParallelFor(TaskScheduler* scheduler, ParallelForKernel fn, void* param, const Range& range)
	{
		parallel_for_internal::ParallelForTaskData task_data[MAX_WORKER_THREAD_COUNT + 1];

		uint32_t part_size, rest;

		uint32_t begin = range.begin;
		uint32_t end = range.end;

		uint32_t count = (end - begin);
		Assert(count);

		// Spread the task so we have one task per thread
		count = Min(count, scheduler->GetWorkerCount() + 1); // +1 as the main thread will perform work

		part_size = (end - begin) / count;
		rest = (end - begin) % count;

		// Define a empty task as a parent to all the sub-tasks
		TaskId parent_task = scheduler->PrepareEmptyTask();


		for (uint32_t i = 0; i < count; ++i)
		{
			uint32_t size = part_size + int(rest > i);

			task_data[i].range = Range(begin, begin + size);
			task_data[i].data = param;
			task_data[i].kernel = fn;

			WorkItem item;
			item.data = &task_data[i];
			item.kernel = parallel_for_internal::ParallelForTaskKernel;

			TaskId sub_task = scheduler->PrepareTask(item);
			scheduler->SetParent(sub_task, parent_task);
			scheduler->SpawnTask(sub_task);

			begin += size;
		}

		// Spawn and wait for the parent task to finish
		//	The parent task won't finish before all sub-tasks are completed
		scheduler->SpawnTaskAndWait(parent_task);
	}

	//-------------------------------------------------------------------------------

} // namespace sb

