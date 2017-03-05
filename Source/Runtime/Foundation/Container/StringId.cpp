// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "StringId.h"
#include "Hash/murmur_hash.h"
#include "StringIdRepository.h"

namespace sb
{

	//-------------------------------------------------------------------------------

	namespace
	{
		StringIdRepository* g_string_id_repository = nullptr;

		INLINE uint32_t StringIdHash32(const char* str, uint32_t len)
		{
			uint32_t hash = murmur_hash_32(str, len, 0);

			if (g_string_id_repository)
				g_string_id_repository->Add(hash, str, len);

			return hash;
		}

		INLINE uint64_t StringIdHash64(const char* str, uint32_t len)
		{
			uint64_t hash = murmur_hash_64(str, len, 0);

			if (g_string_id_repository)
				g_string_id_repository->Add(hash, str, len);

			return hash;
		}
	};

	void string_id::SetRepository(StringIdRepository* repository)
	{
		g_string_id_repository = repository;
	}

	//-------------------------------------------------------------------------------

	StringId32::StringId32()
	{
		_id = 0;
	}
	StringId32::StringId32(const char* str)
	{
		_id = StringIdHash32(str, (uint32_t)strlen(str));
	}
	StringId32::StringId32(const char* str, uint32_t length)
	{
		_id = StringIdHash32(str, length);
	}
	StringId32::StringId32(const string& str)
	{
		_id = StringIdHash32(str.c_str(), (uint32_t)str.size());
	}
	uint32_t StringId32::GetId() const
	{
		return _id;
	}
	bool StringId32::operator==(const StringId32& other) const
	{
		return (_id == other._id);
	}
	bool StringId32::operator!=(const StringId32& other) const
	{
		return (_id != other._id);
	}
	bool StringId32::operator<(const StringId32& other) const
	{
		return (_id < other._id);
	}
	bool StringId32::operator>(const StringId32& other) const
	{
		return (_id > other._id);
	}

	//-------------------------------------------------------------------------------

	StringId64::StringId64()
	{
		_id = 0;
	}
	StringId64::StringId64(const char* str)
	{
		_id = StringIdHash64(str, (uint32_t)strlen(str));
	}
	StringId64::StringId64(const char* str, uint32_t length)
	{
		_id = StringIdHash64(str, length);
	}
	StringId64::StringId64(const string& str)
	{
		_id = StringIdHash64(str.c_str(), (uint32_t)str.size());
	}
	uint64_t StringId64::GetId() const
	{
		return _id;
	}
	bool StringId64::operator==(const StringId64& other) const
	{
		return (_id == other._id);
	}
	bool StringId64::operator!=(const StringId64& other) const
	{
		return (_id != other._id);
	}
	bool StringId64::operator<(const StringId64& other) const
	{
		return (_id < other._id);
	}
	bool StringId64::operator>(const StringId64& other) const
	{
		return (_id > other._id);
	}

	//-------------------------------------------------------------------------------

} // namespace sb

