// Copyright 2008-2014 Simon Ekström

#include "Testing/Framework.h"
#include <Foundation/Container/BitArray.h>

using namespace sb;

TEST_CASE(BitArray_Fill)
{
	BitArray arr;
	arr.Resize(128);
	ASSERT_EQUAL(arr.Size(), 128);
	
	arr.Fill(true);
	for (int i = 0; i < 128; ++i)
	{
		ASSERT_EQUAL(arr[i], true);
	}
}

TEST_CASE(BitArray_PushBack)
{
	BitArray arr;
	for (int i = 0; i < 128; ++i)
	{
		arr.PushBack(true);
	}
	ASSERT_EQUAL(arr.Size(), 128);
	ASSERT_EQUAL(arr[127], true);
}

TEST_CASE(BitArray_Clear)
{
	BitArray arr;
	arr.Resize(128);
	ASSERT_EQUAL(arr.Size(), 128);
	
	arr.Clear();
	ASSERT_EQUAL(arr.Empty(), true);
}

TEST_CASE(BitArray_Copy)
{
	BitArray arr;
	arr.Resize(128);
	arr.Fill(true);

	BitArray arr2(arr);
	ASSERT_EQUAL(arr2.Size(), 128);
	ASSERT_EQUAL(arr2[127], true);
}

TEST_CASE(BitArray_Assignment)
{
	BitArray arr;
	arr.Resize(128);
	arr.Fill(true);

	BitArray arr2;

	arr2 = arr;
	ASSERT_EQUAL(arr2.Size(), 128);
	ASSERT_EQUAL(arr2[127], true);
}

TEST_CASE(BitArray_Modify)
{
	BitArray arr;
	arr.Resize(128);
	arr.Fill(true);

	arr[0] = false;
	ASSERT_EQUAL(arr[0], false);
}

