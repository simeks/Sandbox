// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_SHARED_PTR_H__
#define __FOUNDATION_SHARED_PTR_H__



namespace sb
{

	/// @brief Shared pointer implementation supporting the custom memory allocators.
	template <typename T>
	class SharedPtr
	{
	public:
		SharedPtr() : _ptr(0), _ref_count(0) { }

		SharedPtr(T* ptr)
			: _ptr(ptr),
			_ref_count(ptr ? (new uint32_t) : 0)
		{
			*_ref_count = 1;
		}

		SharedPtr(const SharedPtr& r)
		{
			_ptr = r._ptr;
			_ref_count = r._ref_count;
			if (_ref_count)
				++(*_ref_count);
		}

		~SharedPtr()
		{
			if (_ref_count)
			{
				if (--(*_ref_count) == 0)
				{
					delete _ptr;
					delete _ref_count;
				}
			}
		}

		void Swap(SharedPtr<T>& other)
		{
			sb::Swap(_ptr, other._ptr);
			sb::Swap(_ref_count, other._ref_count);
		}

		void Reset()
		{
			SharedPtr<T>().Swap(*this);
		}

		SharedPtr& operator=(const SharedPtr& r)
		{
			if (_ptr == r.Get())
				return *this;

			SharedPtr<T> tmp(r);
			Swap(tmp);

			return *this;
		}

		T& operator*() const { Assert(_ptr); return *_ptr; }
		T* operator->() const { Assert(_ptr); return _ptr; }
		T* Get() const { return _ptr; }

		uint32_t RefCount() const { Assert(_ref_count); return *_ref_count; }

	private:
		T* _ptr;
		uint32_t* _ref_count;


	};

	template<typename T, typename U> inline bool operator==(SharedPtr<T> const& l, SharedPtr<U> const& r)
	{
		return l.Get() == r.Get();
	}
	template<typename T, typename U> inline bool operator!=(SharedPtr<T> const& l, SharedPtr<U> const& r)
	{
		return l.Get() != r.Get();
	}

} // namespace sb

#endif // __FOUNDATION_SHARED_PTR_H__
