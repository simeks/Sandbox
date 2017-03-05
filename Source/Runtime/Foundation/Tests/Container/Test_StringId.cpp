// Copyright 2008-2014 Simon Ekström

#include "Testing/Framework.h"

#include <Foundation/Container/StringId.h>
#include <Foundation/Container/StringIdRepository.h>

using namespace sb;

TEST_CASE(StringId32_Compare)
{
	String str("string");

	StringId32 id1(str.Ptr());
	StringId32 id2("hash");
	StringId32 id3("string");

	ASSERT_NOT_EQUAL(id1.GetId(), id2.GetId());
	ASSERT_EQUAL(id1.GetId(), id3.GetId());
}

TEST_CASE(StringId64_Compare)
{
	String str("string");

	StringId64 id1(str.Ptr());
	StringId64 id2("hash");
	StringId64 id3("string");

	ASSERT_NOT_EQUAL(id1.GetId(), id2.GetId());
	ASSERT_EQUAL(id1.GetId(), id3.GetId());
}

TEST_CASE(StringId_Repository)
{
	StringIdRepository repo(nullptr, "");
	
	const char* str = "test";
	StringId64 id(str);
	repo.Add(id.GetId(), str, (uint32_t)strlen(str));

	const char* lookup = repo.LookUp(id.GetId());
	ASSERT_EQUAL(str, lookup);
}

