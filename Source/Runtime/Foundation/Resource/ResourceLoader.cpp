// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ResourceLoader.h"
#include "ResourceManager.h"

#include "Filesystem/File.h"
#include "Filesystem/FileSystem.h"
#include "Profiler/Profiler.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	ResourceLoader::ResourceLoader(FileSystem* file_system)
		: _file_system(file_system),
		_worker(_file_system)
	{

	}
	ResourceLoader::~ResourceLoader()
	{
	}
	//-------------------------------------------------------------------------------
	void ResourceLoader::Initialize()
	{
		_worker.Start();
	}
	void ResourceLoader::Shutdown()
	{
		_worker.Stop();
	}
	LoadRequestId ResourceLoader::AddRequest(const Request& request)
	{
		RequestInternal* internal_request = new(_request_pool.Allocate()) RequestInternal;
		Assert(internal_request);

		internal_request->request = request;
		internal_request->processed = 0;

		_worker.PushRequest(internal_request);

		return _request_pool.GetIndex(internal_request);
	}
	bool ResourceLoader::GetResult(LoadRequestId request_id, Result& result)
	{
		RequestInternal* internal_request = _request_pool.GetObject((uint32_t)request_id);
		Assert(internal_request);

		if (internal_request->processed == 0)
		{
			return false; // Request not processed yet
		}

		// Fill in the result
		result.result = internal_request->request.result;

		// Release our request
		internal_request->~RequestInternal();
		_request_pool.Release(internal_request);

		return true;
	}
	bool ResourceLoader::WaitResult(LoadRequestId request_id, Result& result)
	{
		RequestInternal* internal_request = _request_pool.GetObject((uint32_t)request_id);
		Assert(internal_request);

		while (internal_request->processed == 0)
		{
			// Yield and let our loader thread work
			Sleep(0);
		}

		// Fill in the result
		result.result = internal_request->request.result;

		// Release our request
		internal_request->~RequestInternal();
		_request_pool.Release(internal_request);

		return true;
	}
	bool ResourceLoader::CancelRequest(LoadRequestId request_id)
	{
		Assert(request_id != INVALID_LOAD_REQUEST_ID);
		RequestInternal* internal_request = _request_pool.GetObject((uint32_t)request_id);
		Assert(internal_request);

		// Check if request already is processed
		if (internal_request->processed != 0)
		{
			return false;
		}

		// Look for the request in the queue
		if (_worker.RemoveRequest(internal_request))
		{
			// Release request from pool
			internal_request->~RequestInternal();
			_request_pool.Release(internal_request);
			return true;
		}

		// Request not processed or in worker queue, which mean its being processed right now.

		return false;
	}
	//-------------------------------------------------------------------------------

	ResourceLoader::LoadWorker::LoadWorker(FileSystem* file_system)
		: _file_system(file_system), _stopping(0)
	{
	}
	ResourceLoader::LoadWorker::~LoadWorker()
	{
	}
	void ResourceLoader::LoadWorker::PushRequest(RequestInternal* request)
	{
		{
			ScopedLock<CriticalSection> scoped_lock(_request_lock);
			_request_queue.push_back(request);
		}
		// Wake the loader
		_thread_wakeup_event.Set();
	}
	bool ResourceLoader::LoadWorker::PopRequest(RequestInternal** request)
	{
		ScopedLock<CriticalSection> scoped_lock(_request_lock);
		if (_request_queue.empty())
			return false;

		*request = _request_queue.front();
		_request_queue.pop_front();
		return true;
	}
	bool ResourceLoader::LoadWorker::RemoveRequest(RequestInternal* request)
	{
		ScopedLock<CriticalSection> scoped_lock(_request_lock);

		deque<RequestInternal*>::iterator it = std::find(_request_queue.begin(), _request_queue.end(), request);

		if (it != _request_queue.end())
		{
			_request_queue.erase(it);
			return true;
		}

		// Not found, most likely already processed
		return false;
	}
	void ResourceLoader::LoadWorker::Start()
	{
		_stopping = 0;
		// Start the thread that should run this worker
		_thread.Start(this);
	}
	void ResourceLoader::LoadWorker::Stop()
	{
		thread::InterlockedExchange(&_stopping, 1);
		_thread_wakeup_event.Set();

		// Wait for our worker thread to exit
		while (_thread.IsRunning())
		{
			Sleep(1);
		}
	}
	void ResourceLoader::LoadWorker::Run()
	{
		while (_stopping == 0)
		{
			RequestInternal* internal_request = 0;
			if (!PopRequest(&internal_request))
			{
				// If queue is empty: Sleep until notified
				_thread_wakeup_event.Wait();
				continue;
			}
			Assert(internal_request);

			{
				PROFILER_SCOPE("Load resource");

				LoadContext context;
				context.user_data = internal_request->request.user_data;
				if (internal_request->request.file_source)
				{
					if (!internal_request->request.resource_path.empty())
						context.file = internal_request->request.file_source->OpenFile(internal_request->request.resource_path.c_str(), File::READ);
				}
				else
				{
					if (!internal_request->request.resource_path.empty())
						context.file = _file_system->OpenFile(internal_request->request.resource_path.c_str(), File::READ);
				}
				context.result = 0;

				// Process request
				if (internal_request->request.load_callback)
				{
					Assert(context.file.Get());
					internal_request->request.load_callback(context);
				}
				context.file.Reset();

				internal_request->request.result = context.result;
			}
			thread::InterlockedExchange(&internal_request->processed, 1);
		}
	}

} // namespace sb

