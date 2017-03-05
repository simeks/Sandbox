// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_MEMORY_H__
#define __FOUNDATION_MEMORY_H__

/// @brief Creates a new object of type T using the allocator a.
#define SB_NEW(a, T, ...) (new ((a).Allocate(sizeof(T), __alignof(T))) T(__VA_ARGS__))

/// @brief Frees an object p of type T from allocator a.
#define SB_DELETE(a, p) (sb::memory::Delete(a, p))

/// @brief Creates a new array of type T with size N
#define SB_NEW_ARRAY(a, T, N) (sb::memory::NewArray<T>(a, N))

/// @brief Frees an array p created by SB_NEW_ARRAY
#define SB_DELETE_ARRAY(a, p) (sb::memory::DeleteArray(a, p))


namespace sb
{

	class Allocator;

	/// @brief Memory namespace
	namespace memory
	{
		// Default alignment for allocations
		enum { DEFAULT_ALIGNMENT = 8 };

		//-------------------------------------------------------------------------------
		void Initialize();
		void Shutdown();

		Allocator& DefaultAllocator();
		Allocator& ScratchAllocator();
#ifdef SANDBOX_MEMORY_TRACKING
		Allocator& DebugAllocator();
#endif

		//-------------------------------------------------------------------------------

		/// @brief Allocated the specified amount of memory with the specified alignment
		void* Malloc(size_t size, uint32_t alignment = DEFAULT_ALIGNMENT);

		/// @brief Changes the size of the specified memory block
		void* Realloc(void* p, size_t size);

		/// @brief Frees memory previously allocated with Malloc()
		void Free(void* p);

		/// @brief Returns the amount of memory allocted at p
		///
		///	@param p Must be a pointer to memory allocated with Malloc()
		size_t GetAllocatedSize(void* p);


		//-------------------------------------------------------------------------------

		INLINE void* Memmove(void* dest, const void* src, size_t num);
		INLINE int Memcmp(const void* ptr1, const void* ptr2, size_t num);
		INLINE void* Memset(void* ptr, uint8_t value, size_t num);
		INLINE void* Memcpy(void* dest, const void* src, size_t num);

		//-------------------------------------------------------------------------------

		/// @brief Helper function for deleting an object
		template<typename T> void Delete(Allocator& allocator, T* p);

		/// @brief Helper function for allocating arrays with a specified allocator
		template<typename T> T* NewArray(Allocator& allocator, size_t N);

		/// @brief Helper function for freeing arrays with a specified allocator
		template<typename T> void DeleteArray(Allocator& allocator, T* p);

		//-------------------------------------------------------------------------------

		/// @brief Returns the result of advancing pointer p by given number of bytes 
		INLINE void* PointerAdd(void *p, size_t bytes);
		/// @brief Returns the result of advancing pointer p by given number of bytes 
		INLINE const void* PointerAdd(const void *p, size_t bytes);

		/// @brief Returns the result of moving pointer p back by given number of bytes 
		INLINE void* PointerSub(void *p, size_t bytes);
		/// @brief Returns the result of advancing pointer p back by given number of bytes 
		INLINE const void* PointerSub(const void *p, size_t bytes);

		/// @brief Aligns the pointer p to the specified alignment
		INLINE void* AlignForward(void* p, size_t alignment);


		//-------------------------------------------------------------------------------


	}; // namespace memory


	//-------------------------------------------------------------------------------
	void* memory::Memmove(void* dest, const void* src, size_t num)
	{
		return memmove(dest, src, num);
	}
	int memory::Memcmp(const void* ptr1, const void* ptr2, size_t num)
	{
		return memcmp(ptr1, ptr2, num);
	}
	void* memory::Memset(void* ptr, uint8_t value, size_t num)
	{
		return memset(ptr, value, num);
	}
	void* memory::Memcpy(void* dest, const void* src, size_t num)
	{
		return memcpy(dest, src, num);
	}

	//-------------------------------------------------------------------------------

	template<typename T> void memory::Delete(Allocator& allocator, T* p)
	{
		if (p)
		{
			Destruct<T>(p);
			allocator.Free(p);
		}
	}
	template<typename T> T* memory::NewArray(Allocator& allocator, size_t n)
	{
		union
		{
			void*	_p;
			size_t*	_n;
			T*		_obj;
		};

		// Since we need to worry about destruction of the elements we store the number
		//	of elements immediately before our array.
		_p = allocator.Allocate(sizeof(T)* n + sizeof(size_t));
		*_n++ = n;

		for (size_t i = 0; i < n; ++i)
		{
			// Construct our instances with placement new
			new(_obj)T;
			++_obj;
		}

		return _obj - n;
	}
	template<typename T> void memory::DeleteArray(Allocator& allocator, T* p)
	{
		if (p)
		{
			// Retrieve our N that we stored immediately before our array
			size_t* pn = reinterpret_cast<size_t*>(p)-1;
			size_t n = *pn;

			// Call the destructor for all elements
			for (size_t i = 0; i < n; ++i)
			{
				p[i].~T();
			}
			// Free all memory
			allocator.Free(pn);
		}
	}

	//-------------------------------------------------------------------------------

	void* memory::PointerAdd(void *p, size_t bytes)
	{
		return (void*)((char *)p + bytes);
	}
	const void* memory::PointerAdd(const void *p, size_t bytes)
	{
		return (const void*)((const char *)p + bytes);
	}
	void* memory::PointerSub(void *p, size_t bytes)
	{
		return (void*)((char *)p - bytes);
	}
	const void* memory::PointerSub(const void *p, size_t bytes)
	{
		return (const void*)((const char *)p - bytes);
	}
	void* memory::AlignForward(void* p, size_t alignment)
	{
		uintptr_t pi = uintptr_t(p);
		size_t mod = pi % alignment;
		if (mod)
			pi += (alignment - mod);
		return (void*)pi;
	}

	//-------------------------------------------------------------------------------

} // namespace sb

#endif // __FOUNDATION_MEMORY_H__

