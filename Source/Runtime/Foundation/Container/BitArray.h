// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_BITARRAY_H__
#define __FOUNDATION_BITARRAY_H__

namespace sb
{

	class BitArrayElementReference
	{
	public:
		inline BitArrayElementReference(size_t& element, size_t mask);

		inline operator bool() const;
		inline bool operator==(bool value) const;
		inline bool operator==(const BitArrayElementReference& other) const;
		inline bool operator!=(bool value) const;
		inline bool operator!=(const BitArrayElementReference& other) const;
		inline BitArrayElementReference& operator=(bool value);

	private:
		BitArrayElementReference& operator=(const BitArrayElementReference&) { return *this; }

		size_t& _element;
		size_t _mask;
	};

	class BitArray
	{
	public:
		typedef BitArrayElementReference Reference;
		typedef bool ConstReference;

		BitArray(Allocator& allocator = memory::DefaultAllocator());
		BitArray(const BitArray& other);
		~BitArray();

		/// @brief Fills the entire array with the specified value
		void Fill(bool value);

		/// @brief Returns the number of elements in the array
		size_t Size() const;

		/// @brief Returns true if the array is empty
		bool Empty() const;


		/// @brief Resizes the array
		void Resize(size_t size);

		/// @brief Removes all items from the array
		void Clear();

		/// @brief Increases the capacity of the array
		void Reserve(size_t capacity);

		/// @brief Trims the array capacity to match its size
		void Trim();


		/// @brief Returns the value at the specified index
		Reference At(size_t index);

		/// @brief Returns the value at the specified index
		ConstReference At(size_t index) const;


		/// @brief Pushes an item to the end of the array
		void PushBack(bool value);

		/// @brief Pops the last item from the array
		void PopBack();


		BitArray&		operator=(const BitArray& other);
		Reference		operator[](size_t index);
		ConstReference	operator[](size_t index) const;

	private:
		/// @brief Increases the capacity of the array using geometric progression
		///	@param capacity Specified minimum capacity if not 0
		void Grow(size_t capacity = 0);

		/// @brief Changes the capacity and resizes the buffer
		void SetCapacity(size_t new_capacity);

	private:
		Allocator* _allocator;

		size_t* _buffer;
		size_t _size;
		size_t _capacity;

	};

	//-------------------------------------------------------------------------------

	BitArrayElementReference::BitArrayElementReference(size_t& element, size_t mask)
		: _element(element), _mask(mask)
	{
	}

	BitArrayElementReference::operator bool() const
	{
		return (_element & _mask) != 0;
	}
	bool BitArrayElementReference::operator==(bool value) const
	{
		bool this_value = (_element & _mask) != 0;
		return (this_value == value);
	}
	bool BitArrayElementReference::operator==(const BitArrayElementReference& other) const
	{
		bool this_value = (_element & _mask) != 0;
		return (this_value == ((bool)other));
	}
	bool BitArrayElementReference::operator!=(bool value) const
	{
		bool this_value = (_element & _mask) != 0;
		return (this_value != value);
	}
	bool BitArrayElementReference::operator!=(const BitArrayElementReference& other) const
	{
		bool this_value = (_element & _mask) != 0;
		return (this_value != ((bool)other));
	}
	BitArrayElementReference& BitArrayElementReference::operator=(bool value)
	{
		if (value)
			_element |= _mask;
		else
			_element &= ~_mask;

		return *this;
	}

	//-------------------------------------------------------------------------------

} // namespace sb


#endif // __FOUNDATION_BITARRAY_H__