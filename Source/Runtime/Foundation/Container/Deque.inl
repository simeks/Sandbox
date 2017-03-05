// Copyright 2008-2014 Simon Ekström

namespace sb
{

	//-------------------------------------------------------------------------------

	template<typename T>
	DequeIterator<T>::DequeIterator()
	{
	}
	template<typename T>
	DequeIterator<T>::DequeIterator(Deque<T>* deque, size_t index)
		: _deque(deque), _index(index)
	{
	}
	template<typename T>
	T& DequeIterator<T>::operator*() const
	{
		return (*_deque)[_index];
	}
	template<typename T>
	T* DequeIterator<T>::operator->() const
	{
		return &(*_deque)[_index];
	}
	template<typename T>
	DequeIterator<T>& DequeIterator<T>::operator++()
	{
		++_index;
		return *this;
	}
	template<typename T>
	DequeIterator<T> DequeIterator<T>::operator++(int)
	{
		DequeIterator result = *this;
		++_index;
		return result;
	}
	template<typename T>
	DequeIterator<T>& DequeIterator<T>::operator--()
	{
		--_index;
		return *this;
	}
	template<typename T>
	DequeIterator<T> DequeIterator<T>::operator--(int)
	{
		DequeIterator result = *this;
		--_index;
		return result;
	}

	template<typename T>
	bool DequeIterator<T>::operator==(const DequeIterator& other) const
	{
		return (_index == other._index);
	}
	template<typename T>
	bool DequeIterator<T>::operator!=(const DequeIterator& other) const
	{
		return (_index != other._index);
	}
	template<typename T>
	bool DequeIterator<T>::operator<(const DequeIterator& other) const
	{
		return (_index < other._index);
	}
	template<typename T>
	bool DequeIterator<T>::operator>(const DequeIterator& other) const
	{
		return (_index > other._index);
	}
	template<typename T>
	bool DequeIterator<T>::operator<=(const DequeIterator& other) const
	{
		return (_index <= other._index);
	}
	template<typename T>
	bool DequeIterator<T>::operator>=(const DequeIterator& other) const
	{
		return (_index >= other._index);
	}

	//-------------------------------------------------------------------------------

	template<typename T>
	ConstDequeIterator<T>::ConstDequeIterator()
	{
	}
	template<typename T>
	ConstDequeIterator<T>::ConstDequeIterator(const Deque<T>* deque, size_t index)
		: _deque(deque), _index(index)
	{
	}
	template<typename T>
	const T& ConstDequeIterator<T>::operator*() const
	{
		return (*_deque)[_index];
	}
	template<typename T>
	const T* ConstDequeIterator<T>::operator->() const
	{
		return &(*_deque)[_index];
	}

	template<typename T>
	ConstDequeIterator<T>& ConstDequeIterator<T>::operator++()
	{
		++_index;
		return *this;
	}
	template<typename T>
	ConstDequeIterator<T> ConstDequeIterator<T>::operator++(int)
	{
		ConstDequeIterator result = *this;
		++_index;
		return result;
	}
	template<typename T>
	ConstDequeIterator<T>& ConstDequeIterator<T>::operator--()
	{
		--_index;
		return *this;
	}
	template<typename T>
	ConstDequeIterator<T> ConstDequeIterator<T>::operator--(int)
	{
		ConstDequeIterator result = *this;
		--_index;
		return result;
	}

	template<typename T>
	bool ConstDequeIterator<T>::operator==(const ConstDequeIterator& other) const
	{
		return (_index == other._index);
	}
	template<typename T>
	bool ConstDequeIterator<T>::operator!=(const ConstDequeIterator& other) const
	{
		return (_index != other._index);
	}
	template<typename T>
	bool ConstDequeIterator<T>::operator<(const ConstDequeIterator& other) const
	{
		return (_index < other._index);
	}
	template<typename T>
	bool ConstDequeIterator<T>::operator>(const ConstDequeIterator& other) const
	{
		return (_index > other._index);
	}
	template<typename T>
	bool ConstDequeIterator<T>::operator<=(const ConstDequeIterator& other) const
	{
		return (_index <= other._index);
	}
	template<typename T>
	bool ConstDequeIterator<T>::operator>=(const ConstDequeIterator& other) const
	{
		return (_index >= other._index);
	}

	//-------------------------------------------------------------------------------

	template<typename T>
	Deque<T>::Deque(Allocator& allocator)
		: _data(allocator), _offset(0), _size(0)
	{
	}
	template<typename T>
	Deque<T>::Deque(const Deque<T>& other)
		: _data(other._data),
		_offset(other._offset),
		_size(other._size)
	{
	}
	template<typename T>
	Deque<T>::~Deque()
	{
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	typename Deque<T>::Iterator Deque<T>::Begin()
	{
		return Iterator(this, 0);
	}
	template<typename T>
	typename Deque<T>::ConstIterator Deque<T>::Begin() const
	{
		return ConstIterator(this, 0);
	}
	template<typename T>
	typename Deque<T>::Iterator Deque<T>::End()
	{
		return Iterator(this, _size);
	}
	template<typename T>
	typename Deque<T>::ConstIterator Deque<T>::End() const
	{
		return ConstIterator(this, _size);
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	void Deque<T>::PushBack(const T& item)
	{
		size_t new_size = _size + 1;
		if (new_size > _data.Size())
			Grow(new_size);

		ArrayFill(_data.Ptr() + ((_size + _offset) % _data.Size()), item, 1);
		++_size;
	}
	template<typename T>
	void Deque<T>::PopBack()
	{
		--_size;
		ArrayDestruct(_data.Ptr() + ((_size + _offset) % _data.Size()), 1);
	}
	template<typename T>
	void Deque<T>::PushFront(const T& item)
	{
		size_t new_size = _size + 1;
		if (new_size > _data.Size())
			Grow(new_size);

		_offset = (_offset - 1 + _data.Size()) % _data.Size();

		ArrayFill(_data.Ptr() + (_offset % _data.Size()), item, 1);
		++_size;
	}
	template<typename T>
	void Deque<T>::PopFront()
	{
		ArrayDestruct(_data.Ptr() + (_offset % _data.Size()), 1);

		_offset = (_offset + 1) % _data.Size();
		--_size;
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	T& Deque<T>::Front()
	{
		return (*this)[0];
	}
	template<typename T>
	const T& Deque<T>::Front() const
	{
		return (*this)[0];
	}
	template<typename T>
	T& Deque<T>::Back()
	{
		return (*this)[_size - 1];
	}
	template<typename T>
	const T& Deque<T>::Back() const
	{
		return (*this)[_size - 1];
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	typename Deque<T>::Iterator Deque<T>::Insert(Iterator position, const T& item)
	{
		size_t buffer_index = (position._index + _offset) % _data.Size();
		Assert(buffer_index <= _data.Size());
		_data.Insert(buffer_index, item, 1);

		++_size;

		return Iterator(this, position._index);
	}
	template<typename T>
	typename Deque<T>::Iterator Deque<T>::Erase(Iterator position)
	{
		size_t buffer_index = (position._index + _offset) % _data.Size();
		Assert(buffer_index < _data.Size());
		_data.Erase(buffer_index, 1);

		--_size;

		return Iterator(this, position._index);
	}

	//-------------------------------------------------------------------------------
	template<typename T>
	void Deque<T>::Reserve(size_t capacity)
	{
		if (capacity > _data.Size())
			Grow(capacity);
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	void Deque<T>::Clear()
	{
		_data.Clear();
		_offset = 0;
		_size = 0;
	}
	template<typename T>
	size_t Deque<T>::Size() const
	{
		return _size;
	}
	template<typename T>
	bool Deque<T>::Empty() const
	{
		return (_size == 0);
	}
	template<typename T>
	size_t Deque<T>::GetCapacity() const
	{
		return _data.Size();
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	void Deque<T>::Grow(size_t capacity)
	{
		capacity = Max(capacity, (_data.GetCapacity() * 2 + 8));
		SetCapacity(capacity);
	}
	template<typename T>
	void Deque<T>::SetCapacity(size_t new_capacity)
	{
		size_t old_size = _data.Size();
		_data.Resize(new_capacity); // Resize the vector holding our elements

		if (_offset + _size > old_size)
		{
			size_t num_items = old_size - _offset;
			ArrayCopy(_data.Ptr() + (new_capacity - num_items), _data.Ptr() + _offset, num_items);
			_offset += new_capacity - old_size;
		}

	}
	//-------------------------------------------------------------------------------
	template<typename T>
	T& Deque<T>::operator[](size_t index)
	{
		Assert(index < _size);
		return _data[(index + _offset) % _data.Size()];
	}
	template<typename T>
	const T& Deque<T>::operator[](size_t index) const
	{
		Assert(index < _size);
		return _data[(index + _offset) % _data.Size()];
	}
	template<typename T>
	Deque<T>& Deque<T>::operator=(const Deque& other)
	{
		if (this != &other)
		{
			_data = other._data;
			_offset = other._offset;
			_size = other._size;
		}
		return *this;
	}
	//-------------------------------------------------------------------------------


} // namespace sb
