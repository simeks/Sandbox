// Copyright 2008-2014 Simon Ekström

namespace sb
{

	template<typename T>
	size_t str_util::Strlen(const T* str)
	{
		Assert(str);
		if (!str)
			return 0;

		const T* end = str;
		while (*end != (T)0)
		{
			++end;
		}
		return (size_t)(end - str);
	}


	//-------------------------------------------------------------------------------

	template<typename T>
	StringBase<T>::StringBase(Allocator& allocator)
		: _allocator(&allocator), _buffer(allocator)
	{
	}
	template<typename T>
	StringBase<T>::StringBase(const T* str, Allocator& allocator)
		: _allocator(&allocator), _buffer(allocator)
	{
		if (str)
		{
			size_t length = str_util::Strlen(str);
			if (length)
			{
				_buffer.Set(str, length + 1);
			}
		}
	}
	template<typename T>
	StringBase<T>::StringBase(const T* str, size_t length, Allocator& allocator)
		: _allocator(&allocator), _buffer(allocator)
	{
		if (length != 0)
		{
			_buffer.Reserve(length + 1);
			_buffer.Set(str, length);
			_buffer.Append((T)'\0', 1);
		}
	}
	template<typename T>
	StringBase<T>::StringBase(const StringBase& str)
		: _allocator(str._allocator), _buffer(*_allocator)
	{
		size_t length = str.Size();
		if (length != 0)
		{
			_buffer.Reserve(length + 1);
			_buffer.Set(str.Ptr(), length);
			_buffer.Append((T)'\0', 1);
		}
	}
	template<typename T>
	StringBase<T>::~StringBase()
	{
	}

	template<typename T>
	void StringBase<T>::Set(const T* values, size_t count)
	{
		_buffer.Set(values, count);
	}

	template<typename T>
	void StringBase<T>::Append(T character, size_t count)
	{
		if (_buffer.Size() == 0)
		{
			_buffer.Append(character, count);
			_buffer.Append((T)0, 1);
		}
		else
		{
			_buffer.Insert(_buffer.Size() - 1, character, count);
		}
	}
	template<typename T>
	void StringBase<T>::Append(const T* str, size_t length)
	{
		if (str)
		{
			if (length != 0)
			{
				// Make sure the string isn't from the same buffer we try to insert into
				Assert(str < _buffer.Ptr() || str >= (_buffer.Ptr() + _buffer.Size()));

				if (_buffer.Size() == 0)
				{
					// Insert at the beginning with +1 for the \0
					_buffer.AppendArray(str, length + 1);
				}
				else
				{
					// Insert before the \0 byte in the buffer
					_buffer.InsertArray(_buffer.Size() - 1, str, length);
				}
			}
		}
	}
	template<typename T>
	void StringBase<T>::Append(const StringBase<T>& str)
	{
		size_t other_size = str._buffer.Size();
		if (other_size > 1)
		{
			size_t my_size = Size();
			_buffer.Resize(my_size + other_size);
			memcpy(_buffer.Ptr() + my_size, str._buffer.Ptr(), sizeof(T)*other_size);
		}
	}
	template<typename T>
	void StringBase<T>::Insert(size_t pos, const StringBase<T>& str)
	{
		Insert(pos, str.Ptr());
	}

	template<typename T>
	void StringBase<T>::Insert(size_t pos, const char* str)
	{
		Assert(pos <= Size());
		if (!str)
			return;

		size_t len = str_util::Strlen(str);
		if (len != 0)
		{
			// Makes sure we are not trying insert a string into itself
			Assert(str < _buffer.Ptr() || str >= (_buffer.Ptr() + _buffer.Size()));

			if (Size() == 0)
			{
				Append(str, len);
			}
			else
			{
				_buffer.InsertArray(pos, str, len);
			}

		}
	}

	template<typename T>
	void StringBase<T>::Insert(size_t pos, char c, size_t count)
	{
		Assert(pos <= Size());
		if (Size() == 0)
		{
			Append(c);
		}
		else
		{
			_buffer.Insert(pos, c, count);
		}
	}

	template<typename T>
	void StringBase<T>::Erase(size_t pos, size_t len)
	{
		if (len == String::npos)
			len = Size() - pos;

		Assert(pos <= Size());
		Assert(pos + len <= Size());
		_buffer.Erase(pos, len);
	}

	template<typename T>
	void StringBase<T>::Format(const T* fmt, ...)
	{
		Assert(fmt);

		// Get length
		va_list args;
		va_start(args, fmt);
		int length = vsnprintf(NULL, 0, fmt, args);
		va_end(args);

		if (length <= 0)
		{
			_buffer.Resize(0);
			return;
		}

		// Resize
		_buffer.Resize(length + 1);

		va_start(args, fmt);
		vsnprintf(_buffer.Ptr(), length + 1, fmt, args);
		va_end(args);

	}
	template<typename T>
	void StringBase<T>::Clear()
	{
		_buffer.Clear();
	}
	template<typename T>
	void StringBase<T>::Reserve(size_t capacity)
	{
		_buffer.Reserve(capacity);
	}
	template<typename T>
	size_t StringBase<T>::Size() const
	{
		size_t buffer_size = _buffer.Size();
		return (buffer_size == 0 ? 0 : buffer_size - 1);
	}
	template<typename T>
	size_t StringBase<T>::GetCapacity() const
	{
		size_t capacity = _buffer.GetCapacity();
		return (capacity == 0 ? 0 : capacity - 1);
	}

	template<typename T>
	bool StringBase<T>::Empty() const
	{
		// If buffer size is 1 we only have our null terminator, 
		//	which means our string is empty.
		return (_buffer.Size() <= 1);
	}
	template<typename T>
	size_t StringBase<T>::Find(char c, size_t pos) const
	{
		size_t size = Size();
		for (size_t idx = pos; idx != size; ++idx)
		{
			if (_buffer[idx] == c)
				return idx;
		}
		return StringBase::npos;
	}
	template<typename T>
	size_t StringBase<T>::RFind(char c, size_t pos) const
	{
		size_t start = (pos == npos) ? Size() : pos;
		for (size_t idx = start; idx != 0; --idx)
		{
			if (_buffer[idx] == c)
				return idx;
		}
		return StringBase::npos;
	}
	template<typename T>
	StringBase<T> StringBase<T>::Substr(size_t pos, size_t len) const
	{
		StringBase output(*_allocator);
		size_t size = Size();
		Assert(pos <= size);

		if (len == 0)
		{
			return output;
		}

		len = Min(len, size - pos);
		size_t end_pos = pos + len;

		output.Reserve(len);
		for (; pos < end_pos; ++pos)
		{
			output.Append(_buffer[pos]);
		}

		return output;
	}

	template<typename T>
	const T* StringBase<T>::Ptr() const
	{
		static const T empty_str[] = "";
		return (_buffer.Size() ? _buffer.Ptr() : empty_str);
	}

	template<typename T>
	Allocator& StringBase<T>::GetAllocator() const
	{
		return *_allocator;
	}


	template<typename T>
	StringBase<T>& StringBase<T>::operator=(T character)
	{
		_buffer.Clear();
		_buffer.Resize(2);
		_buffer[0] = character;
		_buffer[1] = (T)0;
		return *this;
	}
	template<typename T>
	StringBase<T>& StringBase<T>::operator=(const T* str)
	{
		_buffer.Clear();
		if (str)
		{
			size_t length = str_util::Strlen(str);
			if (length)
			{
				_buffer.Set(str, length + 1);
			}
		}
		return *this;
	}
	template<typename T>
	StringBase<T>& StringBase<T>::operator=(const StringBase& str)
	{
		_buffer = str._buffer;
		return *this;
	}

	template<typename T>
	StringBase<T>& StringBase<T>::operator+=(T character)
	{
		Append(character);
		return *this;
	}
	template<typename T>
	StringBase<T>& StringBase<T>::operator+=(const T* str)
	{
		size_t len = str_util::Strlen(str);
		Append(str, len);
		return *this;
	}
	template<typename T>
	StringBase<T>& StringBase<T>::operator+=(const StringBase<T>& str)
	{
		Append(str);
		return *this;
	}

	template<typename T>
	T& StringBase<T>::operator[](size_t index)
	{
		Assert(index < Size());
		return _buffer[index];
	}
	template<typename T>
	const T& StringBase<T>::operator[](size_t index) const
	{
		Assert(index < Size());
		return _buffer[index];
	}
	template<typename T>
	bool StringBase<T>::operator<(const T* str) const
	{
		if (!str || str[0] == 0)
		{
			return false;
		}

		size_t size = Size();
		for (size_t i = 0; i < size; ++i)
		{
			T a = _buffer[i];
			T b = str[i];
			if (a < b)
			{
				return true;
			}

			if ((b < a) || (b == 0))
			{
				return false;
			}
		}
		return (0 < str[size]);
	}
	template<typename T>
	bool StringBase<T>::operator<(const StringBase<T>& other) const
	{
		size_t size = Min(Size(), other.Size());
		for (size_t i = 0; i < size; ++i)
		{
			T a = _buffer[i];
			T b = other._buffer[i];
			if (a < b)
			{
				return true;
			}

			if ((b < a) || (b == 0))
			{
				return false;
			}
		}
		return (Size() < other.Size());
	}

	template<typename T>
	bool StringBase<T>::operator==(const T* str) const
	{
		// Check if str is empty and matching
		if (!str || str[0] == (T)0)
			return (_buffer.Size() <= 1);

		// Compare each character
		size_t str_length = Size();
		for (size_t i = 0; i < str_length; ++i)
		{
			if (str[i] == (T)0 || str[i] != _buffer[i])
				return false;
		}

		return (str[str_length] == (T)0);
	}
	template<typename T>
	bool StringBase<T>::operator==(const StringBase<T>& other) const
	{
		// First check if size matches
		if (_buffer.Size() == other._buffer.Size())
		{
			// Then compare buffers
			return (memcmp(_buffer.Ptr(), other._buffer.Ptr(), _buffer.Size() * sizeof(T)) == 0);
		}
		return false;
	}
	template<typename T>
	bool StringBase<T>::operator!=(const T* str) const
	{
		return !(*this == str);
	}
	template<typename T>
	bool StringBase<T>::operator!=(const StringBase<T>& other) const
	{
		return !(*this == other);
	}
	//-------------------------------------------------------------------------------

} // namespace sb

