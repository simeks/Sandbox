// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_CONFIGVALUE_H__
#define __FOUNDATION_CONFIGVALUE_H__

namespace sb
{

	class ConfigValueIterator;
	class ConfigValueConstIterator;

	class ConfigValue
	{
	public:
		enum ValueType
		{
			NULL_VALUE,
			INTEGER,
			UINTEGER,
			FLOAT,
			BOOL,
			STRING,
			ARRAY,
			OBJECT
		};

		typedef map<string, ConfigValue> ValueMap;
		typedef ConfigValueIterator Iterator;
		typedef ConfigValueConstIterator ConstIterator;

		typedef vector<ConfigValue> ValueArray;


	public:
		ConfigValue();
		ConfigValue(const ConfigValue& source);
		~ConfigValue();

		ValueType Type() const;

		bool IsNull() const;
		bool IsInt() const;
		bool IsUInt() const;
		bool IsFloat() const;
		bool IsBool() const;
		bool IsString() const;
		bool IsArray() const;
		bool IsObject() const;

		/// @return True if value is a number, meaning either an integer, unsigned integer or float
		bool IsNumber() const;


		int AsInt() const;
		int64_t AsInt64() const;
		uint32_t AsUInt() const;
		uint64_t AsUInt64() const;
		float AsFloat() const;
		double AsDouble() const;
		bool AsBool() const;
		const char* AsString() const;

		/// @brief Returns the size of this value, the number of sub elements
		/// @return Number of sub elements if either an array or an object. 
		///			If the value is a single element type this returns 1, and if
		///			the value is NULL it returns 0.
		uint32_t Size() const;

		void SetNull();
		void SetInt(int i);
		void SetInt(int64_t i);
		void SetUInt(uint32_t i);
		void SetUInt(uint64_t i);
		void SetFloat(float f);
		void SetDouble(double d);
		void SetBool(bool b);
		void SetString(const char* s);

		/// @brief Sets this value to an empty array
		void SetEmptyArray();

		/// @brief Sets this value to an empty object
		void SetEmptyObject();

		/// @brief Appends a value to the array
		/// @remark Assumes that this ConfigValue is an array
		ConfigValue& Append();

		/// @brief Returns an iterator for the beginning of all object elements 
		/// @remark This only works if the value is of the type OBJECT
		Iterator Begin();

		/// @brief Returns an iterator for the beginning of all object elements 
		/// @remark This only works if the value is of the type OBJECT
		ConstIterator Begin() const;

		/// @brief Returns the end iterator for the object elements
		/// @remark This only works if the value is of the type OBJECT
		Iterator End();

		/// @brief Returns the end iterator for the object elements
		/// @remark This only works if the value is of the type OBJECT
		ConstIterator End() const;

		ConfigValue& operator=(const ConfigValue& source);

		ConfigValue& operator[](const char* key);
		const ConfigValue& operator[](const char* key) const;

		ConfigValue& operator[](int index);
		const ConfigValue& operator[](int index) const;

	private:

		union Value
		{
			int64_t i;
			uint64_t u;
			double d;
			bool b;
			string* s;
			ValueArray* a;
			ValueMap* o;
		};

		ValueType _type;
		Value _value;

	};

	class ConfigValueIterator : public ConfigValue::ValueMap::iterator
	{
	public:
		ConfigValueIterator() : ConfigValue::ValueMap::iterator()  {}
		ConfigValueIterator(ConfigValue::ValueMap::iterator iter) :
			ConfigValue::ValueMap::iterator(iter) { }
	};

	class ConfigValueConstIterator : public ConfigValue::ValueMap::const_iterator
	{
	public:
		ConfigValueConstIterator() : ConfigValue::ValueMap::const_iterator()  {}
		ConfigValueConstIterator(ConfigValue::ValueMap::const_iterator iter) :
			ConfigValue::ValueMap::const_iterator(iter) { }
	};


} // namespace sb


#endif // __FOUNDATION_CONFIGVALUE_H__
