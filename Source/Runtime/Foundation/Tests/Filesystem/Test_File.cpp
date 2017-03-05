// Copyright 2008-2014 Simon Ekström

#include "Testing/Framework.h"

#include <Foundation/Filesystem/FileSystem.h>
#include <Foundation/Filesystem/FileSource.h>
#include <Foundation/Filesystem/FileUtil.h>
#include <Foundation/Filesystem/File.h>

using namespace sb;



TEST_CASE(File_MakeDirectory)
{
	FileSystem file_system("./");
	file_system.MakeDirectory("test");

	vector<string> dirs;
	file_util::FindDirectories("./test", dirs);
	bool dir_found = !dirs.empty();

	ASSERT_EXPR(dir_found);
}

TEST_CASE(File_Write)
{
	FileSystem file_system("./");

	FileSource* file_source = file_system.OpenFileSource("test");
	{
		FileStreamPtr file = file_source->OpenFile("test_file", File::WRITE);
		ASSERT_EXPR(file.Get() != NULL);

		size_t written = file->Write("string", 7);

		ASSERT_EQUAL(written, 7);
	}
	vector<string> files;
	file_util::FindFiles("./test/test_file", files);
	bool file_found = !files.empty();

	ASSERT_EXPR(file_found);
}

TEST_CASE(File_Read)
{
	FileSystem file_system("./");
	
	FileSource* file_source = file_system.OpenFileSource("test");
	
	FileStreamPtr file = file_source->OpenFile("test_file", File::READ);
	ASSERT_EXPR(file.Get() != NULL);

	char str[7];
	size_t read = file->Read(str, 7);
	
	ASSERT_EQUAL(read, 7);

	ASSERT_EQUAL_STR(str, "string");
}

TEST_CASE(File_Length)
{
	FileSystem file_system("./");
	
	FileSource* file_source = file_system.OpenFileSource("test");
	FileStreamPtr file = file_source->OpenFile("test_file", File::READ);
	ASSERT_EXPR(file.Get() != NULL);
	
	int64_t length = file->Length();
	
	ASSERT_EQUAL(length, 7);
}

