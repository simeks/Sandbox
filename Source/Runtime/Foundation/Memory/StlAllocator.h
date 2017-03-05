// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_STLALLOCATOR_H__
#define __FOUNDATION_STLALLOCATOR_H__


namespace sb
{

	/// @brief Custom allocator for STL
	///
	///	It's made so that we can use our memory::DebugAllocator with STL 
	template<typename T>
	class stl_debug_allocator : public std::allocator<T>
	{
	public:
		typedef T          value_type;
		typedef size_t     size_type;
		typedef ptrdiff_t  difference_type;

		typedef T*         pointer;
		typedef const T*   const_pointer;

		typedef T&         reference;
		typedef const T&   const_reference;

		template<typename U>
		struct rebind
		{
			typedef stl_debug_allocator<U> other;
		};

		stl_debug_allocator() throw() {}
		stl_debug_allocator(const stl_debug_allocator&) throw() {}
		template<typename U> stl_debug_allocator(const stl_debug_allocator<U>&) throw() {}
		~stl_debug_allocator() throw() {}

		pointer allocate(size_type n, const void* = 0)
		{
#ifdef SANDBOX_MEMORY_TRACKING
			return (pointer)memory::DebugAllocator().Allocate(n*sizeof(T));
#else
			return (pointer)memory::DefaultAllocator().Allocate(n*sizeof(T));
#endif
		}
		void deallocate(pointer p, size_type)
		{
#ifdef SANDBOX_MEMORY_TRACKING
			memory::DebugAllocator().Free(p);
#else
			memory::DefaultAllocator().Free(p);
#endif
		}

	};

	template <typename T, typename U>
	inline bool operator==(const stl_debug_allocator<T>&, const stl_debug_allocator<U>){ return true; }

	template <typename T, typename U>
	inline bool operator!=(const stl_debug_allocator<T>&, const stl_debug_allocator<U>){ return false; }

} // namespace sb

#endif // __FOUNDATION_STLALLOCATOR_H__
