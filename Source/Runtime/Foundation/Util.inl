// Copyright 2008-2014 Simon Ekström

namespace sb
{

	//-------------------------------------------------------------------------------
	namespace util_internal
	{
		/// @brief Copies an array to a specified memory region
		/// The implementation for this method is for type without a trivial assignment operator
		template<typename T>
		void ArrayMove(T* dst, const T* src, size_t count, const std::false_type&)
		{
			// Object with without a trival assignment operator

			if (dst <= src)
			{
				for (size_t i = 0; i < count; ++i)
				{
					dst[i] = src[i];
				}
			}
			else
			{
				for (size_t i = (count); i >= 1; --i)
				{
					dst[i - 1] = src[i - 1];
				}
			}
		}

		/// @brief Copies an array to a specified memory region
		/// The implementation for this method is for type with a trivial assignment operator
		template<typename T>
		void ArrayMove(T* dst, const T* src, size_t count, const std::true_type&)
		{
			// Object with a trival assignment operator
			memory::Memmove(dst, src, sizeof(T)*count);
		}


		/// @brief Constructs copies of object from the source array to the destination buffer
		/// The implementation of this method is for types without a trivial copy constructor
		template<typename T>
		void ArrayCopy(T* dst, const T* src, size_t count, const std::false_type&)
		{
			// Object without a trival copy constructor
			for (size_t i = 0; i < count; ++i)
			{
				new(dst + i) T(src[i]);
			}
		}

		/// @brief Constructs copies of object from the source array to the destination buffer
		/// The implementation of this method is for types with a trivial copy constructor
		template<typename T>
		void ArrayCopy(T* dst, const T* src, size_t count, const std::true_type&)
		{
			// Object with a trival copy constructor
			memcpy(dst, src, sizeof(T)*count);
		}


		/// @brief Constructs a number of elements in an array
		/// The implementation of this method is for types without a trivial constructor
		template<typename T>
		void ArrayConstruct(T* arr, size_t count, const std::false_type&)
		{
			// Object without a trival constructor
			T* obj = arr;
			for (size_t i = 0; i < count; ++i)
			{
				new(obj)T;
				++obj;
			}
		}

		/// @brief Constructs a number of elements in an array
		/// The implementation of this method is for types with a trivial constructor
		template<typename T>
		void ArrayConstruct(T*, size_t, const std::true_type&)
		{
			// Object with a trival constructor: Nothing needs to be done
		}

		/// @brief Destructs a number of elements in an array
		/// The implementation of this method is for types without a trivial destructor
		template<typename T>
		void ArrayDestruct(T* arr, size_t count, const std::false_type&)
		{
			// Object without a trival destructor
			for (size_t i = 0; i < count; ++i)
			{
				arr[i].~T();
			}
		}

		/// @brief Destructs a number of elements in an array
		/// The implementation of this method is for types with a trivial destructor
		template<typename T>
		void ArrayDestruct(T*, size_t, const std::true_type&)
		{
			// Object with a trival destructor: Nothing needs to be done
		}


		/// @brief Destructs an object.
		/// The implementation of this method is for types without a trivial destructor
		template<typename T>
		void Destruct(T* arr, const std::false_type&)
		{
			arr->~T();
		}

		/// @brief Destructs an object.
		/// The implementation of this method is for types with a trivial destructor
		template<typename T>
		void Destruct(T*, const std::true_type&)
		{
			// Object with a trival destructor: Nothing needs to be done
		}

	}; // namespace util_internal

	//-------------------------------------------------------------------------------


	template<typename T>
	void ArrayFill(T* dst, const T& src, size_t count)
	{
		for (size_t i = 0; i < count; ++i)
		{
			new(dst + i) T(src);
		}
	}
	template<typename T>
	void ArrayMove(T* dst, const T* src, size_t count)
	{
		util_internal::ArrayMove(dst, src, count, std::has_trivial_assign<T>());
	}

	template<typename T>
	void ArrayCopy(T* dst, const T* src, size_t count)
	{
		util_internal::ArrayCopy(dst, src, count, std::has_trivial_copy<T>());
	}

	template<typename T>
	void ArrayConstruct(T* arr, size_t count)
	{
		util_internal::ArrayConstruct<T>(arr, count, std::has_trivial_constructor<T>());
	}

	template<typename T>
	void ArrayDestruct(T* arr, size_t count)
	{
		util_internal::ArrayDestruct<T>(arr, count, std::has_trivial_destructor<T>());
	}

	template<typename T>
	void Destruct(T* arr)
	{
		util_internal::Destruct<T>(arr, std::has_trivial_destructor<T>());
	}

	//-------------------------------------------------------------------------------
	template<typename T>
	INLINE void Swap(T& l, T& r)
	{
		if (&l != &r)
		{
			T tmp = l;
			l = r;
			r = tmp;
		}
	}

	template<typename T>
	INLINE const T& (Min)(const T& a, const T& b)
	{
		return ((a < b) ? a : b);
	}
	template<typename T>
	INLINE const T& (Max)(const T& a, const T& b)
	{
		return ((a > b) ? a : b);
	}

	//-------------------------------------------------------------------------------

	template<typename T> T Invalid()
	{
		return static_cast<T>(~static_cast<T>(0));
	}
	template<typename T> bool IsValid(T index)
	{
		return (index != Invalid<T>());
	}
	template<typename T> bool IsInvalid(T index)
	{
		return (index == Invalid<T>());
	}
	template<typename T> void SetInvalid(T& index)
	{
		index = Invalid<T>();
	}


	//-------------------------------------------------------------------------------

	template<typename T>
	bool Less<T>::operator()(const T& a, const T& b) const
	{
		return (a < b);
	}

	//-------------------------------------------------------------------------------

	template<typename I, typename T> I Find(I first, I last, const T& value)
	{
		for (; first != last; ++first)
		{
			if (*first == value)
			{
				return first;
			}
		}
		return last;
	}

	//-------------------------------------------------------------------------------

} // namespace sb


