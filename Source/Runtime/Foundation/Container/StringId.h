// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_STRINGID_H__
#define __FOUNDATION_STRINGID_H__

namespace sb
{

	/// @brief string identifier, an uint32_t representing a string in the system
	///	
	///	StringId is a string identifier, it is a hashed string that gets calculated
	///		with murmur_hash_32 and stored as an uint32_t. 
	class StringId32
	{
		uint32_t _id;

	public:
		StringId32();
		StringId32(const char* str);
		StringId32(const char* str, uint32_t length);
		StringId32(const string& str);

		uint32_t GetId() const;

		bool operator==(const StringId32& other) const;
		bool operator!=(const StringId32& other) const;
		bool operator<(const StringId32& other) const;
		bool operator>(const StringId32& other) const;
	};

	/// @brief string identifier, an uint64_t representing a string in the system
	///	
	///	StringId is a string identifier, it is a hashed string that gets calculated
	///		with murmur_hash_64 and stored as an uint64_t. 
	class StringId64
	{
		uint64_t _id;

	public:
		StringId64();
		StringId64(const char* str);
		StringId64(const char* str, uint32_t length);
		StringId64(const string& str);

		uint64_t GetId() const;

		bool operator==(const StringId64& other) const;
		bool operator!=(const StringId64& other) const;
		bool operator<(const StringId64& other) const;
		bool operator>(const StringId64& other) const;
	};

	class StringIdRepository;
	namespace string_id
	{
		/// Sets a repository were all string identifiers created will be recorded.
		///		Set to NULL to inactivate recording.
		void SetRepository(StringIdRepository* repository);
	};


} // namespace sb



#endif // __FOUNDATION_STRINGID_H__
