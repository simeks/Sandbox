// Copyright 2008-2014 Simon Ekström

namespace sb
{

	//-------------------------------------------------------------------------------

	template<typename T>
	VectorIterator<T>::VectorIterator()
	{
	}
	template<typename T>
	VectorIterator<T>::VectorIterator(T* object) : _object(object)
	{
	}
	template<typename T>
	T& VectorIterator<T>::operator*() const
	{
		return *_object;
	}
	template<typename T>
	T* VectorIterator<T>::operator->() const
	{
		return _object;
	}
	template<typename T>
	VectorIterator<T>& VectorIterator<T>::operator++()
	{
		++_object;
		return *this;
	}
	template<typename T>
	VectorIterator<T> VectorIterator<T>::operator++(int)
	{
		VectorIterator result = *this;
		++_object;
		return result;
	}
	template<typename T>
	VectorIterator<T>& VectorIterator<T>::operator--()
	{
		--_object;
		return *this;
	}
	template<typename T>
	VectorIterator<T> VectorIterator<T>::operator--(int)
	{
		VectorIterator result = *this;
		--_object;
		return result;
	}

	template<typename T>
	bool VectorIterator<T>::operator==(const VectorIterator& other) const
	{
		return (_object == other._object);
	}
	template<typename T>
	bool VectorIterator<T>::operator!=(const VectorIterator& other) const
	{
		return (_object != other._object);
	}
	template<typename T>
	bool VectorIterator<T>::operator<(const VectorIterator& other) const
	{
		return (_object < other._object);
	}
	template<typename T>
	bool VectorIterator<T>::operator>(const VectorIterator& other) const
	{
		return (_object > other._object);
	}
	template<typename T>
	bool VectorIterator<T>::operator<=(const VectorIterator& other) const
	{
		return (_object <= other._object);
	}
	template<typename T>
	bool VectorIterator<T>::operator>=(const VectorIterator& other) const
	{
		return (_object >= other._object);
	}

	//-------------------------------------------------------------------------------

	template<typename T>
	ConstVectorIterator<T>::ConstVectorIterator()
	{
	}
	template<typename T>
	ConstVectorIterator<T>::ConstVectorIterator(const T* object) : _object(object)
	{
	}

	template<typename T>
	const T& ConstVectorIterator<T>::operator*() const
	{
		return *_object;
	}
	template<typename T>
	const T* ConstVectorIterator<T>::operator->() const
	{
		return _object;
	}

	template<typename T>
	ConstVectorIterator<T>& ConstVectorIterator<T>::operator++()
	{
		++_object;
		return *this;
	}
	template<typename T>
	ConstVectorIterator<T> ConstVectorIterator<T>::operator++(int)
	{
		ConstVectorIterator result = *this;
		++_object;
		return result;
	}
	template<typename T>
	ConstVectorIterator<T>& ConstVectorIterator<T>::operator--()
	{
		--_object;
		return *this;
	}
	template<typename T>
	ConstVectorIterator<T> ConstVectorIterator<T>::operator--(int)
	{
		ConstVectorIterator result = *this;
		--_object;
		return result;
	}

	template<typename T>
	bool ConstVectorIterator<T>::operator==(const ConstVectorIterator& other) const
	{
		return (_object == other._object);
	}
	template<typename T>
	bool ConstVectorIterator<T>::operator!=(const ConstVectorIterator& other) const
	{
		return (_object != other._object);
	}
	template<typename T>
	bool ConstVectorIterator<T>::operator<(const ConstVectorIterator& other) const
	{
		return (_object < other._object);
	}
	template<typename T>
	bool ConstVectorIterator<T>::operator>(const ConstVectorIterator& other) const
	{
		return (_object > other._object);
	}
	template<typename T>
	bool ConstVectorIterator<T>::operator<=(const ConstVectorIterator& other) const
	{
		return (_object <= other._object);
	}
	template<typename T>
	bool ConstVectorIterator<T>::operator>=(const ConstVectorIterator& other) const
	{
		return (_object >= other._object);
	}

	//-------------------------------------------------------------------------------
	template<typename T>
	Vector<T>::Vector(Allocator& allocator)
		: _allocator(&allocator), _buffer(0), _size(0), _capacity(0)
	{
	}
	template<typename T>
	Vector<T>::Vector(const Vector<T>& other)
		: _allocator(other._allocator), _buffer(0), _size(0), _capacity(0)
	{
		CopyConstruct(other);
	}
	template<typename T>
	Vector<T>::~Vector()
	{
		Finalize();
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	typename Vector<T>::Iterator Vector<T>::Begin()
	{
		return Iterator(_buffer);
	}
	template<typename T>
	typename Vector<T>::ConstIterator Vector<T>::Begin() const
	{
		return ConstIterator(_buffer);
	}
	template<typename T>
	typename Vector<T>::Iterator Vector<T>::End()
	{
		return Iterator(_buffer + _size);
	}
	template<typename T>
	typename Vector<T>::ConstIterator Vector<T>::End() const
	{
		return ConstIterator(_buffer + _size);
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	size_t Vector<T>::Size() const
	{
		return _size;
	}
	template<typename T>
	size_t Vector<T>::GetCapacity() const
	{
		return _capacity;
	}

	template<typename T>
	bool Vector<T>::Empty() const
	{
		return (_size == 0);
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	void Vector<T>::Resize(size_t size)
	{
		if (size == _size)
			return;

		if (size < _size)
		{
			ArrayDestruct(_buffer + size, _size - size);
		}
		else
		{
			Grow(size);
			ArrayConstruct(_buffer + _size, size - _size);
		}
		_size = size;
	}
	template<typename T>
	void Vector<T>::Clear()
	{
		if (0 < _size)
		{
			ArrayDestruct(_buffer, _size);
		}
		_size = 0;
	}
	template<typename T>
	void Vector<T>::Reserve(size_t capacity)
	{
		if (capacity > _capacity)
		{
			SetCapacity(capacity);
		}
	}
	template<typename T>
	void Vector<T>::Trim()
	{
		SetCapacity(_size);
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	void Vector<T>::PushBack(const T& item)
	{
		Append(item, 1);
	}
	template<typename T>
	void Vector<T>::PopBack()
	{
		Assert(_size != 0);
		Erase(_size - 1);
	}
	//-------------------------------------------------------------------------------	
	template<typename T>
	void Vector<T>::Set(const T* values, size_t count)
	{
		Assert(values);
		ArrayDestruct(_buffer, _size);
		if (count != _capacity)
		{
			_buffer = (T*)_allocator->Reallocate(_buffer, count);
			_capacity = count;
		}

		ArrayCopy(_buffer, values, count);
		_size = count;
	}
	template<typename T>
	void Vector<T>::Append(const T& value, size_t count)
	{
		size_t new_size = _size + count;
		if (new_size > _capacity)
			Grow(new_size);

		ArrayFill(_buffer + _size, value, count);

		_size = new_size;
	}
	template<typename T>
	void Vector<T>::AppendArray(const T* values, size_t count)
	{
		Assert(values);
		size_t new_size = _size + count;
		if (new_size >= _capacity)
			Grow(new_size);

		ArrayCopy(_buffer + _size, values, count);

		_size = new_size;
	}

	template<typename T>
	void Vector<T>::Insert(size_t index, const T& value, size_t count)
	{
		size_t new_size = _size + count;
		if (new_size >= _capacity)
		{
			// Buffer is too small so we need to allocate more memory
			size_t new_capacity = Max(new_size, (_capacity * 2 + 8));
			T* new_buffer = (T*)_allocator->Allocate(sizeof(T)*new_capacity, __alignof(T));
			Assert(new_buffer);

			// Copy elements from < index to the new buffer
			ArrayCopy(new_buffer, _buffer, index);

			// Set the new values at index <= i < index + count
			ArrayFill((new_buffer + index), value, count);

			// Copy the old elements from >= index to the end of the buffer
			ArrayCopy((new_buffer + index + count), (_buffer + index), (_size - index));

			_allocator->Free(_buffer);

			_buffer = new_buffer;
			_capacity = new_capacity;
		}
		else
		{
			// Copy the old elements from >= index to the end of the buffer
			ArrayCopy((_buffer + index + count), (_buffer + index), (_size - index));
			// Set our new values at index
			ArrayFill(_buffer + index, value, count);

		}
		_size = new_size;
	}
	template<typename T>
	void Vector<T>::InsertArray(size_t index, const T* values, size_t count)
	{
		Assert(index <= _size);
		Assert(values);
		size_t new_size = _size + count;
		if (new_size >= _capacity)
		{
			// Buffer is to small so we need to allocate more memory
			size_t new_capacity = Max(new_size, (_capacity * 2 + 8));
			T* new_buffer = (T*)_allocator->Allocate(sizeof(T)*new_capacity, __alignof(T));
			Assert(new_buffer);

			// Copy elements from < index to the new buffer
			ArrayCopy(new_buffer, _buffer, index);
			// Set the new values at index <= i < index + count
			ArrayCopy((new_buffer + index), values, count);
			// Copy the old elements from >= index to the end of the buffer
			ArrayCopy((new_buffer + index + count), (_buffer + index), (_size - index));

			_allocator->Free(_buffer);

			_buffer = new_buffer;
			_capacity = new_capacity;
		}
		else
		{
			// Copy the old elements from >= index to the end of the buffer
			ArrayCopy((_buffer + index + count), (_buffer + index), (_size - index));
			// Set our new values at index
			ArrayCopy((_buffer + index), values, count);

		}
		_size = new_size;
	}
	template<typename T>
	typename Vector<T>::Iterator Vector<T>::Erase(size_t index, size_t count)
	{
		Assert(index <= _size);
		Assert(index + count <= _size);

		size_t new_size = _size - count;
		ArrayMove(_buffer + index, _buffer + index + count, (_size - index - count));
		ArrayDestruct(_buffer + new_size, count);
		_size = new_size;

		return Iterator(_buffer + index);
	}
	template<typename T>
	typename Vector<T>::Iterator Vector<T>::Erase(Iterator position)
	{
		size_t index = &(*position) - _buffer;

		size_t new_size = _size - 1;
		ArrayMove(_buffer + index, _buffer + index + 1, (_size - index - 1));
		ArrayDestruct(_buffer + new_size, 1);
		_size = new_size;

		return Iterator(_buffer + index);
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	T& Vector<T>::At(size_t index)
	{
		Assert(index < _size);
		return _buffer[index];
	}
	template<typename T>
	const T& Vector<T>::At(size_t index) const
	{
		Assert(index < _size);
		return _buffer[index];
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	T& Vector<T>::Front()
	{
		Assert(_size != 0);
		return _buffer[0];
	}
	template<typename T>
	const T& Vector<T>::Front() const
	{
		Assert(_size != 0);
		return _buffer[0];
	}
	template<typename T>
	T& Vector<T>::Back()
	{
		Assert(_size != 0);
		return _buffer[_size - 1];
	}
	template<typename T>
	const T& Vector<T>::Back() const
	{
		Assert(_size != 0);
		return _buffer[_size - 1];
	}
	template<typename T>
	T* Vector<T>::Ptr()
	{
		return _buffer;
	}
	template<typename T>
	const T* Vector<T>::Ptr() const
	{
		return _buffer;
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	Vector<T>& Vector<T>::operator=(const Vector<T>& other)
	{
		if (this != &other)
		{
			Finalize();
			CopyConstruct(other);
		}
		return *this;
	}

	template<typename T>
	T& Vector<T>::operator[](size_t index)
	{
		Assert(index < _size);
		return _buffer[index];
	}
	template<typename T>
	const T& Vector<T>::operator[](size_t index) const
	{
		Assert(index < _size);
		return _buffer[index];
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	void Vector<T>::Finalize()
	{
		if (!_buffer)
			return;

		ArrayDestruct(_buffer, _size);
		_allocator->Free(_buffer);
		_buffer = 0;
		_size = 0;
		_capacity = 0;
	}
	template<typename T>
	void Vector<T>::CopyConstruct(const Vector<T>& other)
	{
		Assert(_buffer == nullptr);

		_size = other._size;
		_capacity = other._size;

		if (_size)
		{
			_buffer = (T*)_allocator->Allocate(sizeof(T)*_capacity, __alignof(T));

			Assert(other._buffer != nullptr);
			ArrayCopy(_buffer, other._buffer, _size);
		}
	}
	template<typename T>
	void Vector<T>::Grow(size_t capacity)
	{
		if (capacity > _capacity)
		{
			capacity = Max(capacity, (_capacity * 2 + 8));
			SetCapacity(capacity);
		}
	}
	template<typename T>
	void Vector<T>::SetCapacity(size_t new_capacity)
	{
		if (new_capacity == _capacity)
			return;
		if (new_capacity < _size)
		{
			ArrayDestruct(_buffer + new_capacity, _size - new_capacity);
			_size = new_capacity;
		}

		T* new_data = 0;
		if (new_capacity > 0)
		{
			new_data = (T*)_allocator->Allocate(sizeof(T)*new_capacity, __alignof(T));
			ArrayCopy(new_data, _buffer, _size);
		}
		ArrayDestruct(_buffer, _size);
		_allocator->Free(_buffer);
		_buffer = new_data;

		_capacity = new_capacity;
	}
	//-------------------------------------------------------------------------------

} // namespace sb




