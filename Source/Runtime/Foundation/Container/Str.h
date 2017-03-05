// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_STRING_H__
#define __FOUNDATION_STRING_H__

#include "Vector.h"

namespace sb
{

	namespace str_util
	{
		/// @brief Returns the length of a C-style string
		template<typename T>
		size_t Strlen(const T* str);

	};

	/// @brief Base string class
	///	@tparam T Character type (E.g. char or wchar_t)
	template<typename T>
	class StringBase
	{
	public:
		static const size_t npos = (size_t)-1;

	protected:
		Allocator*	_allocator;
		Vector<T>	_buffer;

	public:
		StringBase(Allocator& allocator = memory::DefaultAllocator());
		explicit StringBase(const T* str, Allocator& allocator = memory::DefaultAllocator());
		StringBase(const T* str, size_t length, Allocator& allocator = memory::DefaultAllocator());

		/// Copy constructor
		StringBase(const StringBase& str);

		/// Destructor
		~StringBase();

		/// @brief Sets this array to a copy of the specified C-style array
		///	@param values Values to copy
		///	@param count Number of values
		void Set(const T* values, size_t count);

		void Append(T character, size_t count = 1);

		/// @brief Appends an C-style string to this string
		///	@param length Length of string, not counting the null terminator
		void Append(const T* str, size_t length);
		void Append(const StringBase<T>& str);

		/// @brief Inserts the specified string into the specified position
		void Insert(size_t pos, const StringBase<T>& str);

		/// @brief Inserts the specified c-string into the specified position
		void Insert(size_t pos, const char* str);

		/// @brief Inserts the specified character into the specified position
		void Insert(size_t pos, char c, size_t count = 1);

		/// Erases a part of the string
		/// @param pos Position of the character to erase
		/// @param len Number of characters to erase, if set to String::npos,
		///			all characters until the end of the string will be erased.
		void Erase(size_t pos, size_t len = npos);


		void Format(const T* fmt, ...);

		/// @brief Clears the string
		void Clear();

		/// @brief Increases the capacity of the string buffer
		void Reserve(size_t capacity);


		/// @brief Returns the size of the string
		size_t Size() const;

		/// @brief Returns the currenct capacity of this string
		size_t GetCapacity() const;

		/// @brief Returns true if the string is empty
		bool Empty() const;

		/// @brief Searches the string for the specified character
		///	@param pos Index from which to start searching
		///	@return The position of the first occurrence or StringBase::npos if not found
		size_t Find(char c, size_t pos = 0) const;

		/// @brief Searches the string for the last occurrence of the specified character
		///	@param pos Index from which to start searching
		///	@return The position of the first occurrence or StringBase::npos if not found
		size_t RFind(char c, size_t pos = npos) const;

		/// @brief Creates a new substring out of the specified portion of this string
		StringBase Substr(size_t pos = 0, size_t len = npos) const;

		const T* Ptr() const;

		Allocator& GetAllocator() const;

		// Operators

		StringBase& operator=(T character);
		StringBase& operator=(const T* str);
		StringBase& operator=(const StringBase& str);

		StringBase& operator+=(T character);
		StringBase& operator+=(const T* str);
		StringBase& operator+=(const StringBase<T>& str);

		T& operator[](size_t index);
		const T& operator[](size_t index) const;

		bool operator<(const T* str) const;
		bool operator<(const StringBase<T>& other) const;

		bool operator==(const T* str) const;
		bool operator==(const StringBase<T>& other) const;
		bool operator!=(const T* str) const;
		bool operator!=(const StringBase<T>& other) const;
	};

	typedef StringBase<char> String;


} // namespace sb



#include "Str.inl"

#endif // __FOUNDATION_STRING_H__
