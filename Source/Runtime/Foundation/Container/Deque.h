// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_DEQUE_H__
#define __FOUNDATION_DEQUE_H__

namespace sb
{

	template<typename T>
	class Deque;

	/// @brief Non-constant iterator for Deque
	template<typename T>
	class DequeIterator
	{
	public:
		DequeIterator();
		DequeIterator(Deque<T>* deque, size_t index);

		T& operator*() const;
		T* operator->() const;

		DequeIterator& operator++();
		DequeIterator operator++(int);
		DequeIterator& operator--();
		DequeIterator operator--(int);

		bool operator==(const DequeIterator& other) const;
		bool operator!=(const DequeIterator& other) const;
		bool operator<(const DequeIterator& other) const;
		bool operator>(const DequeIterator& other) const;
		bool operator<=(const DequeIterator& other) const;
		bool operator>=(const DequeIterator& other) const;

	protected:
		friend Deque<T>;

		Deque<T>* _deque;
		size_t _index;

	};

	/// @brief Constant iterator for Deque
	template<typename T>
	class ConstDequeIterator
	{
	public:
		ConstDequeIterator();
		ConstDequeIterator(const Deque<T>* deque, size_t index);

		const T& operator*() const;
		const T* operator->() const;

		ConstDequeIterator& operator++();
		ConstDequeIterator operator++(int);
		ConstDequeIterator& operator--();
		ConstDequeIterator operator--(int);

		bool operator==(const ConstDequeIterator& other) const;
		bool operator!=(const ConstDequeIterator& other) const;
		bool operator<(const ConstDequeIterator& other) const;
		bool operator>(const ConstDequeIterator& other) const;
		bool operator<=(const ConstDequeIterator& other) const;
		bool operator>=(const ConstDequeIterator& other) const;

	protected:
		friend Deque<T>;

		const Deque<T>* _deque;
		size_t _index;

	};


	/// @brief A double-ended queue.
	template<typename T>
	class Deque
	{
	public:
		typedef DequeIterator<T> Iterator;
		typedef ConstDequeIterator<T> ConstIterator;

	public:
		Deque(Allocator& allocator = memory::DefaultAllocator());
		Deque(const Deque& other);

		~Deque();

		/// @brief Returns an iterator to the beginning of this queue
		Iterator Begin();

		/// @brief Returns an const iterator to the beginning of this queue
		ConstIterator Begin() const;

		/// @brief Returns an iterator to the end of this queue
		Iterator End();

		/// @brief Returns an const iterator to the end of this queue
		ConstIterator End() const;


		/// @brief Pushes an item to the end of the queue
		void PushBack(const T& item);

		/// @brief Pops the last item from the queue
		void PopBack();

		/// @brief Pushes an item to the front of the queue
		void PushFront(const T& item);

		/// @brief Pops the front item from the queue
		void PopFront();


		/// @brief Returns the first element in the queue.
		T& Front();

		/// @brief Returns the first element in the queue.
		const T& Front() const;

		/// @brief Returns the last element in the queue.
		T& Back();

		/// @brief Returns the last element in the queue.
		const T& Back() const;


		/// Inserts an element at the specified position
		/// @return Iterator pointing to the newly inserted element.
		Iterator Insert(Iterator position, const T& value);

		/// Removes the specified element from the queue
		/// @return Iterator pointing to the element following the removed element.
		Iterator Erase(Iterator position);

		/// @brief Increases the capacity of the array
		/// @param capacity The minimum capacity this container should have reserved. 
		void Reserve(size_t capacity);


		/// @brief Removes all elements from the queue
		void Clear();

		/// @brief Returns the number of elements in the queue
		size_t Size() const;

		/// @brief Returns true if the queue is empty
		bool Empty() const;

		/// @brief Returns the current capacity of this container
		size_t GetCapacity() const;

		T& operator[](size_t index);
		const T& operator[](size_t index) const;
		Deque& operator=(const Deque& other);

	private:
		/// @brief Increases the capacity of the array using geometric progression
		///	@param capacity Specified minimum capacity if not 0
		void Grow(size_t capacity = 0);

		/// @brief Changes the capacity and resizes the buffer
		void SetCapacity(size_t new_capacity);

	private:
		Vector<T> _data;

		size_t _offset;
		size_t _size;
	};

} // namespace sb

#include "Deque.inl"

#endif // __FOUNDATION_DEQUE_H__
