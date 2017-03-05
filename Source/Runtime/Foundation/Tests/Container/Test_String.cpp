// Copyright 2008-2014 Simon Ekström

#include "Testing/Framework.h"

#include <Foundation/Container/Str.h>

using namespace sb;

TEST_CASE(String_Constructor)
{
	String str("string");
	ASSERT_EQUAL(str.Size(), 6);
	ASSERT_EQUAL_STR(str.Ptr(), "string");
}

TEST_CASE(String_Append)
{
	String str("str");
	str += "ing";
	
	ASSERT_EQUAL(str.Size(), 6);
	ASSERT_EQUAL_STR(str.Ptr(), "string");

	str.Append("string", 6);
	ASSERT_EQUAL(str.Size(), 12);
	ASSERT_EQUAL_STR(str.Ptr(), "stringstring");
}

TEST_CASE(String_Format)
{
	String str;
	str.Format("%d%d%d%d", 1,2,3,4);
	ASSERT_EQUAL(str.Ptr(), "1234");
}

TEST_CASE(String_Insert)
{
	String str("string");;
	str.Insert(3, 'i', 2);
	ASSERT_EQUAL_STR(str.Ptr(), "striiing");
}

TEST_CASE(String_Copy)
{
	String str("string");
	String str2(str);
	ASSERT_EQUAL_STR(str2.Ptr(), "string");
}


TEST_CASE(String_Assignment)
{
	String str("string");
	String str2;
	str2 = str;
	ASSERT_EQUAL_STR(str2.Ptr(), "string");
}
