// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_FILEPATH_H__
#define __FOUNDATION_FILEPATH_H__

namespace sb
{

	class FilePath
	{
	public:
		FilePath();
		FilePath(const char* path);
		FilePath(const string& path);
		~FilePath();

		void Set(const char* path);
		void Set(const string& path);
		void Clear();

		/// Converts the path to use the specified separator
		void SetSeparator(char c);

		string Directory() const;
		string Filename() const;
		string Extension() const;

		/// Removes the extension from the path
		void TrimExtension();

		/// Returns the full path
		string& Get();

		/// Returns the full path
		const string& Get() const;

		const char* c_str() const;

		FilePath& operator+=(const string& other);
		FilePath& operator+=(const char* other);
		FilePath& operator+=(const char other);

	private:
		string _path;

	};

} // namespace sb


#endif // __FOUNDATION_FILEPATH_H__
