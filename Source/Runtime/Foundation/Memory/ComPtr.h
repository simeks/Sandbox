// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_COM_PTR_H__
#define __FOUNDATION_COM_PTR_H__


namespace sb
{

	/// @brief Shared pointer implementation supporting the custom memory allocators.
	template <typename T>
	class ComPtr
	{
	public:
		ComPtr() : _ptr(nullptr) {}
		ComPtr(T* ptr) : _ptr(ptr)
		{
			if (_ptr)
				_ptr->AddRef();
		}
		ComPtr(const ComPtr& r)
		{
			_ptr = r._ptr;
			if (_ptr)
				_ptr->AddRef();
		}

		~ComPtr()
		{
			if (_ptr)
				_ptr->Release();
			_ptr = nullptr;
		}

		T* operator=(T* ptr)
		{
			if (_ptr)
				_ptr->Release();

			_ptr = ptr;
			if (_ptr)
				_ptr->AddRef();

			return _ptr;
		}

		T* operator=(const ComPtr<T>& ptr)
		{
			if (_ptr)
				_ptr->Release();

			_ptr = ptr._ptr;

			if (_ptr)
				_ptr->AddRef();

			return _ptr;
		}

		T** operator&() { return &_ptr; }
		T& operator*() const { Assert(_ptr); return *_ptr; }
		T* operator->() const { Assert(_ptr); return _ptr; }
		T* Get() const { return _ptr; }

		bool operator!() const
		{
			return (_ptr == nullptr);
		}

		bool operator!=(T* ptr) const
		{
			return _ptr != ptr;
		}

		bool operator==(T* ptr) const
		{
			return _ptr == ptr;
		}

	private:
		T* _ptr;

	};

	template<typename T, typename U> inline bool operator==(ComPtr<T> const& l, ComPtr<U> const& r)
	{
		return l.Get() == r.Get();
	}
	template<typename T, typename U> inline bool operator!=(ComPtr<T> const& l, ComPtr<U> const& r)
	{
		return l.Get() != r.Get();
	}

} // namespace sb


#endif // __FOUNDATION_COM_PTR_H__
