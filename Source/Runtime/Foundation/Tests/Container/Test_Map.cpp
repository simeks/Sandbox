// Copyright 2008-2014 Simon Ekström

#include "Testing/Framework.h"

#include <Foundation/Container/Map.h>

using namespace sb;

TEST_CASE(Map_Iterator)
{
	Map<int, int> map;
	map[0] = 0;
	map[1] = 1;
	map[2] = 2;

	ASSERT_EQUAL(map.Size(), 3);

	Map<int, int>::Iterator it, end;
	it = map.Begin(); end = map.End();
	for(int i = 0; it != end; ++it, ++i)
	{
		ASSERT_EQUAL(it->first, i);
		ASSERT_EQUAL(it->second, i);
	}
}

TEST_CASE(Map_Find)
{
	Map<int, int> map;
	map[0] = 0;
	map[1] = 1;
	map[2] = 2;

	Map<int, int>::Iterator it = map.Find(1);
	ASSERT_EQUAL(it->first, 1);
	ASSERT_EQUAL(it->second, 1);
}

TEST_CASE(Map_Insert)
{
	Map<int, int> map;
	map[0] = 0;
	map[1] = 1;
	map[2] = 2;

	map.Insert(Pair<int, int>(10, 10));

	ASSERT_EQUAL(map[10], 10);
}

TEST_CASE(Map_Erase)
{
	Map<int, int> map;
	map[0] = 0;
	map[1] = 1;
	map[2] = 2;

	ASSERT_EQUAL(map.Erase(1), 1);
	ASSERT_EQUAL(map.Size(), 2);
}

TEST_CASE(Map_Copy)
{
	Map<int, int> map;
	map[0] = 0; map[1] = 1; map[2] = 2;

	Map<int, int> map2;
	map2 = map;
	
	ASSERT_EQUAL(map2.Size(), 3);
	ASSERT_EQUAL(map2[2], 2);
}

TEST_CASE(Map_Assignment)
{
	Map<int, int> map;
	map[0] = 0; map[1] = 1; map[2] = 2;

	Map<int, int> map2(map);
	
	ASSERT_EQUAL(map2.Size(), 3);
	ASSERT_EQUAL(map2[2], 2);
}
