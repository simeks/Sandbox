// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_UTIL_H__
#define __FOUNDATION_UTIL_H__

/// @file Util.h
/// @brief Common utilities


namespace sb
{

	/// Swaps two types
	template<typename T> void Swap(T& l, T& r);
	/// Returns the smallest of two values
	template<typename T> const T& (Min)(const T& a, const T& b);
	/// Returns the biggest of two values
	template<typename T> const T& (Max)(const T& a, const T& b);

	/// @brief Constructs copies of object from the source value to the destination buffer
	template<typename T>
	void ArrayFill(T* dst, const T& src, size_t count);

	/// @brief Copies an array to a specified memory region
	template<typename T>
	void ArrayMove(T* dst, const T* src, size_t count);

	/// @brief Constructs copies of object from the source array to the destination buffer
	template<typename T>
	void ArrayCopy(T* dst, const T* src, size_t count);

	/// @brief Constructs a number of elements in an array
	template<typename T>
	void ArrayConstruct(T* arr, size_t count);

	/// @brief Destructs a number of elements in an array
	template<typename T>
	void ArrayDestruct(T* arr, size_t count);

	/// @brief Destructs an object
	template<typename T>
	void Destruct(T* arr);


	/// Returns an invalid index for the specified type
	template<typename T> T Invalid();
	/// Checks if the specified index is valid
	template<typename T> bool IsValid(T index);
	/// Checks if the specified index is invalid
	template<typename T> bool IsInvalid(T index);
	/// Invalidates an index
	template<typename T> void SetInvalid(T& index);


	/// Default function class for less than
	template<typename T>
	class Less
	{
	public:
		bool operator()(const T& a, const T& b) const;
	};

	/// Searches for the specfied value between the two iterators.
	///	@return Iterator to the first element that compares to value. Returns the last iterator if no such element was found.
	template<typename I, typename T> I Find(I first, I last, const T& value);

} // namespace sb



#include "Util.inl"

#endif // __FOUNDATION_UTIL_H__