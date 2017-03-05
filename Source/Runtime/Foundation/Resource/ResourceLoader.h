// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_RESOURCELOADER_H__
#define __FOUNDATION_RESOURCELOADER_H__

#include <Foundation/Filesystem/FileStream.h>


namespace sb
{

	typedef uint32_t LoadRequestId;
	static const LoadRequestId INVALID_LOAD_REQUEST_ID = 0xffffffffu;

	class FileStream;
	class FileSource;
	class FileSystem;
	class RenderResourceAllocator;

	/// @brief ResourceLoader is used to load resources on a separate thread
	class ResourceLoader : NonCopyable
	{
	public:
		struct LoadContext
		{
			void* user_data;

			StringId64 resource_id; ///< String id from resource name
			FileStreamPtr file;

			void* result; ///< Result from the load operation
		};

		struct UnloadContext
		{
			void* user_data;
			void* resource_data;
		};

		typedef void(*LoadFn)(ResourceLoader::LoadContext&);

		/// @brief Load request
		struct Request
		{
			Request() {}

			string resource_path;
			FileSource* file_source;

			// Callbacks
			LoadFn load_callback;
			void* user_data;

			void* result; // Output data

		};

		struct Result
		{
			void* result;
		};

	public:
		ResourceLoader(FileSystem* file_system);
		~ResourceLoader();

		void Initialize();
		void Shutdown();

		LoadRequestId AddRequest(const Request& request);

		/// @brief Tries to get the result from the specified request 
		///	@param result The result with be stored in the struct
		///	@return True if the result was ready, false if request isn't processed yet
		bool GetResult(LoadRequestId request_id, Result& result);

		/// @brief Stalls the calling thread until the specified request has been processed
		///	@param result The result with be stored in the struct
		///	@return True if the result is ready to use, false if the load failed
		bool WaitResult(LoadRequestId request_id, Result& result);

		/// @brief Tries to cancel a request
		///
		///	This tries to cancel a pending request, it will return true if successful or true 
		///		if the specified request already is processed or if it can't be find.
		///	If this return false the caller himself must make sure to retrieve the data from
		///		the loader and unload it.
		bool CancelRequest(LoadRequestId request_id);


	private:

		struct RequestInternal
		{
			Request request;
			// Set to non-zero value to mark this request as processed
			volatile long processed;
		};


		/// @brief Worker thread for loading resources
		class LoadWorker : public Runnable
		{
			SimpleThread _thread;
			Event _thread_wakeup_event;

			FileSystem* _file_system;

			CriticalSection _request_lock;
			deque<RequestInternal*> _request_queue;

			volatile long _stopping; //<! If the manager is shutting down

		public:
			LoadWorker(FileSystem* file_system);
			~LoadWorker();

			/// @brief Tries to remove a request from the queue
			///	@return True if the request was found and removed
			bool RemoveRequest(RequestInternal* request);

			void PushRequest(RequestInternal* request);
			bool PopRequest(RequestInternal** request);

			/// @brief Starts the worker on its own thread
			void Start();

			/// @brief Signal the worker to stop
			void Stop();

			// Runnable

			void Run();

			/// @brief Is the thread running? 
			bool IsRunning() const { return _thread.IsRunning(); }

		};

		FileSystem* _file_system;

		MemoryPool<RequestInternal, 128> _request_pool;
		LoadWorker _worker;

	};

} // namespace sb



#endif // __FOUNDATION_RESOURCELOADER_H__