// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ConfigValue.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	ConfigValue::ConfigValue()
		: _type(NULL_VALUE)
	{
	}
	ConfigValue::ConfigValue(const ConfigValue& source)
		: _type(source._type)
	{
		switch (_type)
		{
		case NULL_VALUE:
		case INTEGER:
		case UINTEGER:
		case FLOAT:
		case BOOL:
			_value = source._value;
			break;
		case STRING:
			_value.s = new string(*source._value.s);
			break;
		case ARRAY:
			_value.a = new ValueArray(*source._value.a);
			break;
		case OBJECT:
			_value.o = new ValueMap(*source._value.o);
			break;
		};
	}
	ConfigValue::~ConfigValue()
	{
		SetNull();
	}
	//-------------------------------------------------------------------------------
	ConfigValue::ValueType ConfigValue::Type() const
	{
		return _type;
	}
	bool ConfigValue::IsNull() const
	{
		return (_type == NULL_VALUE);
	}
	bool ConfigValue::IsInt() const
	{
		return (_type == INTEGER);
	}
	bool ConfigValue::IsUInt() const
	{
		return (_type == UINTEGER);
	}
	bool ConfigValue::IsFloat() const
	{
		return (_type == FLOAT);
	}
	bool ConfigValue::IsBool() const
	{
		return (_type == BOOL);
	}
	bool ConfigValue::IsString() const
	{
		return (_type == STRING);
	}
	bool ConfigValue::IsArray() const
	{
		return (_type == ARRAY);
	}
	bool ConfigValue::IsObject() const
	{
		return (_type == OBJECT);
	}
	bool ConfigValue::IsNumber() const
	{
		return (_type == INTEGER || _type == UINTEGER || _type == FLOAT);
	}
	//-------------------------------------------------------------------------------
	int ConfigValue::AsInt() const
	{
		switch (_type)
		{
		case NULL_VALUE:
			return 0;
		case INTEGER:
			return int(_value.i);
		case UINTEGER:
			return int(_value.u);
		case FLOAT:
			return int(_value.d);
		case BOOL:
			return (_value.b ? 1 : 0);
		case STRING:
		case ARRAY:
		case OBJECT:
			Assert(false);
		};
		return 0;
	}
	int64_t ConfigValue::AsInt64() const
	{
		switch (_type)
		{
		case NULL_VALUE:
			return 0;
		case INTEGER:
			return _value.i;
		case UINTEGER:
			return int64_t(_value.u);
		case FLOAT:
			return int64_t(_value.d);
		case BOOL:
			return (_value.b ? 1 : 0);
		case STRING:
		case ARRAY:
		case OBJECT:
			Assert(false);
		};
		return 0;
	}
	uint32_t ConfigValue::AsUInt() const
	{
		switch (_type)
		{
		case NULL_VALUE:
			return 0;
		case INTEGER:
			return uint32_t(_value.i);
		case UINTEGER:
			return uint32_t(_value.u);
		case FLOAT:
			return int(_value.d);
		case BOOL:
			return (_value.b ? 1 : 0);
		case STRING:
		case ARRAY:
		case OBJECT:
			Assert(false);
		};
		return 0;
	}
	uint64_t ConfigValue::AsUInt64() const
	{
		switch (_type)
		{
		case NULL_VALUE:
			return 0;
		case INTEGER:
			return uint64_t(_value.i);
		case UINTEGER:
			return uint64_t(_value.u);
		case FLOAT:
			return uint64_t(_value.d);
		case BOOL:
			return (_value.b ? 1 : 0);
		case STRING:
		case ARRAY:
		case OBJECT:
			Assert(false);
		};
		return 0;
	}
	float ConfigValue::AsFloat() const
	{
		switch (_type)
		{
		case NULL_VALUE:
			return 0.0f;
		case INTEGER:
			return float(_value.i);
		case UINTEGER:
			return float(_value.u);
		case FLOAT:
			return float(_value.d);
		case BOOL:
			return (_value.b ? 1.0f : 0.0f);
		case STRING:
		case ARRAY:
		case OBJECT:
			Assert(false);
		};
		return 0.0f;
	}
	double ConfigValue::AsDouble() const
	{
		switch (_type)
		{
		case NULL_VALUE:
			return 0.0;
		case INTEGER:
			return double(_value.i);
		case UINTEGER:
			return double(_value.u);
		case FLOAT:
			return _value.d;
		case BOOL:
			return (_value.b ? 1.0 : 0.0);
		case STRING:
		case ARRAY:
		case OBJECT:
			Assert(false);
		};
		return 0.0;
	}
	bool ConfigValue::AsBool() const
	{
		switch (_type)
		{
		case NULL_VALUE:
			return false;
		case INTEGER:
			return _value.i != 0;
		case UINTEGER:
			return _value.u != 0;
		case FLOAT:
			return _value.d != 0.0;
		case BOOL:
			return _value.b;
		case STRING:
		case ARRAY:
		case OBJECT:
			Assert(false);
		};
		return false;
	}
	const char* ConfigValue::AsString() const
	{
		Assert(_type == STRING);
		return _value.s->c_str();
	}
	uint32_t ConfigValue::Size() const
	{
		switch (_type)
		{
		case INTEGER:
		case UINTEGER:
		case FLOAT:
		case BOOL:
			return 1;
		case STRING:
			return (uint32_t)_value.s->size();
		case ARRAY:
			return (uint32_t)_value.a->size();
		case OBJECT:
			return (uint32_t)_value.o->size();
		case NULL_VALUE:
			return 0;
		};
		return 0;
	}
	//-------------------------------------------------------------------------------
	void ConfigValue::SetNull()
	{
		switch (_type)
		{
		case NULL_VALUE:
		case INTEGER:
		case UINTEGER:
		case FLOAT:
		case BOOL:
			break;
		case STRING:
			delete _value.s;
			break;
		case ARRAY:
			delete _value.a;
			break;
		case OBJECT:
			delete _value.o;
			break;
		};
		_type = NULL_VALUE;
		_value.i = 0;
	}
	void ConfigValue::SetInt(int i)
	{
		switch (_type)
		{
		case NULL_VALUE:
		case INTEGER:
		case UINTEGER:
		case FLOAT:
		case BOOL:
			break;
		case STRING:
		case ARRAY:
		case OBJECT:
			SetNull();
			break;
		};
		_type = INTEGER;
		_value.i = i;
	}
	void ConfigValue::SetInt(int64_t i)
	{
		switch (_type)
		{
		case NULL_VALUE:
		case INTEGER:
		case UINTEGER:
		case FLOAT:
		case BOOL:
			break;
		case STRING:
		case ARRAY:
		case OBJECT:
			SetNull();
			break;
		};
		_type = INTEGER;
		_value.i = i;
	}
	void ConfigValue::SetUInt(uint32_t i)
	{
		switch (_type)
		{
		case NULL_VALUE:
		case INTEGER:
		case UINTEGER:
		case FLOAT:
		case BOOL:
			break;
		case STRING:
		case ARRAY:
		case OBJECT:
			SetNull();
			break;
		};
		_type = UINTEGER;
		_value.u = i;
	}
	void ConfigValue::SetUInt(uint64_t i)
	{
		switch (_type)
		{
		case NULL_VALUE:
		case INTEGER:
		case UINTEGER:
		case FLOAT:
		case BOOL:
			break;
		case STRING:
		case ARRAY:
		case OBJECT:
			SetNull();
			break;
		};
		_type = UINTEGER;
		_value.u = i;
	}
	void ConfigValue::SetFloat(float f)
	{
		switch (_type)
		{
		case NULL_VALUE:
		case INTEGER:
		case UINTEGER:
		case FLOAT:
		case BOOL:
			break;
		case STRING:
		case ARRAY:
		case OBJECT:
			SetNull();
			break;
		};
		_type = FLOAT;
		_value.d = f;
	}
	void ConfigValue::SetDouble(double d)
	{
		switch (_type)
		{
		case NULL_VALUE:
		case INTEGER:
		case UINTEGER:
		case FLOAT:
		case BOOL:
			break;
		case STRING:
		case ARRAY:
		case OBJECT:
			SetNull();
			break;
		};
		_type = FLOAT;
		_value.d = d;
	}
	void ConfigValue::SetBool(bool b)
	{
		switch (_type)
		{
		case NULL_VALUE:
		case INTEGER:
		case UINTEGER:
		case FLOAT:
		case BOOL:
			break;
		case STRING:
		case ARRAY:
		case OBJECT:
			SetNull();
			break;
		};
		_type = BOOL;
		_value.b = b;
	}
	void ConfigValue::SetString(const char* s)
	{
		switch (_type)
		{
		case NULL_VALUE:
		case INTEGER:
		case UINTEGER:
		case FLOAT:
		case BOOL:
			break;
		case ARRAY:
		case OBJECT:
			SetNull();
			break;
		case STRING:
			_value.s->clear();
			*_value.s = s;
			return;
		};

		_type = STRING;
		_value.s = new string(s);
		Assert(_value.s);
		*_value.s = s;
	}
	void ConfigValue::SetEmptyArray()
	{
		switch (_type)
		{
		case NULL_VALUE:
		case INTEGER:
		case UINTEGER:
		case FLOAT:
		case BOOL:
			break;
		case STRING:
		case OBJECT:
			SetNull();
			break;
		case ARRAY:
			_value.a->clear();
			return;
		};

		_type = ARRAY;
		_value.a = new ValueArray();
		Assert(_value.a);
	}
	void ConfigValue::SetEmptyObject()
	{
		switch (_type)
		{
		case NULL_VALUE:
		case INTEGER:
		case UINTEGER:
		case FLOAT:
		case BOOL:
			break;
		case STRING:
		case ARRAY:
			SetNull();
			break;
		case OBJECT:
			_value.o->clear();
			return;
		};

		_type = OBJECT;
		_value.o = new ValueMap();
		Assert(_value.o);
	}
	ConfigValue& ConfigValue::Append()
	{
		Assert(_type == ARRAY);
		_value.a->push_back(ConfigValue());
		return _value.a->back();
	}

	ConfigValue::Iterator ConfigValue::Begin()
	{
		Assert(_type == OBJECT);
		return Iterator(_value.o->begin());
	}
	ConfigValue::ConstIterator ConfigValue::Begin() const
	{
		Assert(_type == OBJECT);
		return ConstIterator(_value.o->begin());
	}
	ConfigValue::Iterator ConfigValue::End()
	{
		Assert(_type == OBJECT);
		return Iterator(_value.o->end());
	}
	ConfigValue::ConstIterator ConfigValue::End() const
	{
		Assert(_type == OBJECT);
		return ConstIterator(_value.o->end());
	}
	//-------------------------------------------------------------------------------
	ConfigValue& ConfigValue::operator=(const ConfigValue& source)
	{
		SetNull();

		_type = source._type;
		switch (_type)
		{
		case NULL_VALUE:
		case INTEGER:
		case UINTEGER:
		case FLOAT:
		case BOOL:
			_value = source._value;
			break;
		case STRING:
			_value.s = new string();
			Assert(_value.s);
			*_value.s = *source._value.s;
			break;
		case ARRAY:
			_value.a = new ValueArray();
			Assert(_value.a);
			*_value.a = *source._value.a;
			break;
		case OBJECT:
			_value.o = new ValueMap();
			Assert(_value.o);
			*_value.o = *source._value.o;
			break;
		};

		return *this;
	}
	ConfigValue& ConfigValue::operator[](const char* key)
	{
		Assert(_type == OBJECT);
		string key_str = key;

		ValueMap::iterator it = _value.o->find(key_str);
		if (it != _value.o->end())
		{
			return it->second;
		}

		it = _value.o->insert(pair<string, ConfigValue>(key_str, ConfigValue())).first;

		return it->second;
	}
	const ConfigValue& ConfigValue::operator[](const char* key) const
	{
		Assert(_type == OBJECT);
		string key_str = key;

		ValueMap::iterator it = _value.o->find(key_str);
		if (it != _value.o->end())
		{
			return it->second;
		}

		it = _value.o->insert(pair<string, ConfigValue>(key_str, ConfigValue())).first;

		return it->second;
	}

	ConfigValue& ConfigValue::operator[](int index)
	{
		Assert(_type == ARRAY);
		return (*_value.a)[index];
	}
	const ConfigValue& ConfigValue::operator[](int index) const
	{
		Assert(_type == ARRAY);
		return (*_value.a)[index];
	}
	//-------------------------------------------------------------------------------

} // namespace sb

