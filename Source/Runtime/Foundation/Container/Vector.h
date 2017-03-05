// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_VECTOR_H__
#define __FOUNDATION_VECTOR_H__

namespace sb
{

	/// @brief Iterator for Vector
	template<typename T>
	class VectorIterator
	{
	public:
		VectorIterator();
		explicit VectorIterator(T* object);

		T& operator*() const;
		T* operator->() const;

		VectorIterator& operator++();
		VectorIterator operator++(int);
		VectorIterator& operator--();
		VectorIterator operator--(int);

		bool operator==(const VectorIterator& other) const;
		bool operator!=(const VectorIterator& other) const;
		bool operator<(const VectorIterator& other) const;
		bool operator>(const VectorIterator& other) const;
		bool operator<=(const VectorIterator& other) const;
		bool operator>=(const VectorIterator& other) const;

	private:
		T* _object;

	};

	/// @brief Constant iterator for Vector
	template<typename T>
	class ConstVectorIterator
	{
	public:
		ConstVectorIterator();
		explicit ConstVectorIterator(const T* object);

		const T& operator*() const;
		const T* operator->() const;

		ConstVectorIterator& operator++();
		ConstVectorIterator operator++(int);
		ConstVectorIterator& operator--();
		ConstVectorIterator operator--(int);

		bool operator==(const ConstVectorIterator& other) const;
		bool operator!=(const ConstVectorIterator& other) const;
		bool operator<(const ConstVectorIterator& other) const;
		bool operator>(const ConstVectorIterator& other) const;
		bool operator<=(const ConstVectorIterator& other) const;
		bool operator>=(const ConstVectorIterator& other) const;

	private:
		const T* _object;
	};

	/// @brief Dynamically resizable array for both POD objects and objects with constructors and destructors
	template<typename T>
	class Vector
	{
	public:
		typedef VectorIterator<T> Iterator;
		typedef ConstVectorIterator<T> ConstIterator;


	public:
		Vector(Allocator& allocator = memory::DefaultAllocator());
		Vector(const Vector& other);

		~Vector();

		/// @brief Returns an iterator to the beginning of this array
		Iterator Begin();

		/// @brief Returns an const iterator to the beginning of this array
		ConstIterator Begin() const;

		/// @brief Returns an iterator to the end of this array
		Iterator End();

		/// @brief Returns an const iterator to the end of this array
		ConstIterator End() const;




		/// @brief Returns the number of elements in the array
		size_t Size() const;

		/// @brief Returns the current capacity of this array
		size_t GetCapacity() const;

		/// @brief Returns true if the array is empty
		bool Empty() const;


		/// @brief Resizes the array
		void Resize(size_t size);

		/// @brief Removes all items from the array
		void Clear();

		/// @brief Increases the capacity of the array
		/// @param capacity The minimum capacity this container should have reserved.  
		void Reserve(size_t capacity);

		/// @brief Trims the array capacity to match its size
		void Trim();


		/// @brief Pushes an item to the end of the array
		void PushBack(const T& item);

		/// @brief Pops the last item from the array
		void PopBack();

		/// @brief Sets this array to a copy of the specified C-style array
		///	@param values	Values to copy
		///	@param count	Number of values
		void Set(const T* values, size_t count);

		/// @brief Appends a number of elements to the end of this array
		///	@param value Value of all new elements
		///	@param count Number of elements to add
		void Append(const T& value, size_t count);

		/// @brief Appends a C-style array to the end of this array.
		///	@param values Array of values to add
		///	@param count Number of elements to add
		void AppendArray(const T* values, size_t count);

		/// @brief Inserts a number of elements to the end of this array
		///	@param index Index at which to insert values
		///	@param value Value of all new elements
		///	@param count Number of elements to add
		void Insert(size_t index, const T& value, size_t count);

		/// @brief Inserts a C-style array to the end of this array.
		///	@param index Index at which to insert values
		///	@param values Array of values to add
		///	@param count Number of elements to add
		void InsertArray(size_t index, const T* values, size_t count);

		/// @brief Removes a number of element at a specified index
		/// @param index Index from where to remove elements
		/// @param count Number of elements to remove
		/// @return Iterator pointing to the element following the removed element.
		Iterator Erase(size_t index, size_t count = 1);

		/// @brief Removes an element at the specified position
		/// @return Iterator pointing to the element following the removed element.
		Iterator Erase(Iterator position);


		/// @brief Returns the value at the specified index
		T& At(size_t index);

		/// @brief Returns the value at the specified index
		const T& At(size_t index) const;


		/// @brief Returns the first item in the array
		T& Front();
		/// @brief Returns the first item in the array
		const T& Front() const;

		/// @brief Returns the last item in the array 
		T& Back();
		/// @brief Returns the last item in the array 
		const T& Back() const;

		/// @brief Returns a pointer to the array buffer
		T* Ptr();
		const T* Ptr() const;


		Vector&		operator=(const Vector& other);
		T&			operator[](size_t index);
		const T&	operator[](size_t index) const;

	private:
		/// @brief Increases the capacity of the array using geometric progression
		///	@param capacity Specified minimum capacity if not 0
		void Grow(size_t capacity = 0);

		/// @brief Changes the capacity and resizes the buffer
		void SetCapacity(size_t new_capacity);


		/// @brief Frees any allocated memory
		void Finalize();
		/// @brief Copy constructor
		void CopyConstruct(const Vector<T>& other);

	private:
		Allocator* _allocator;

		T* _buffer;
		size_t _size;
		size_t _capacity;

	public:
		INLINE Iterator begin() 
		{ 
			return Begin(); 
		}
		INLINE ConstIterator begin() const 
		{ 
			return Begin(); 
		}
		INLINE Iterator end() 
		{ 
			return End();
		}
		INLINE ConstIterator end() const 
		{ 
			return End();
		}
	};

} // namespace sb

#include "Vector.inl"

#endif // __FOUNDATION_VECTOR_H__
