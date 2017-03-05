// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_STRINGIDREPOSITORY_H__
#define __FOUNDATION_STRINGIDREPOSITORY_H__

namespace sb
{

	class FileSource;
	class StringIdRepository
	{
	public:
		/// @param target_path Where to save the registry.
		StringIdRepository(FileSource* file_source, const char* target_path);
		~StringIdRepository();

		/// Saves the registry to file
		void Save();
		// Tries to load repository from file
		bool Load();

		void Add(uint32_t id, const char* string, uint32_t len);
		void Add(uint64_t id, const char* string, uint32_t len);

		/// Returns a string matching the given identifier, returns NULL if no match was found.SS
		const char* LookUp(uint32_t id) const;

		/// Returns a string matching the given identifier, returns NULL if no match was found.SS
		const char* LookUp(uint64_t id) const;

	private:
		const StringIdRepository& operator=(const StringIdRepository&) { return *this; }

		map<uint32_t, string> _strings_32;
		map<uint64_t, string> _strings_64;

		FileSource* _file_source; // File source to save registry in
		string _target_path;

	};

} // namespace sb


#endif // __FOUNDATION_STRINGIDREPOSITORY_H__
