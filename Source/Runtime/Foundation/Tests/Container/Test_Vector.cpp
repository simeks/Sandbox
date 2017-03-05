// Copyright 2008-2014 Simon Ekström

#include "Testing/Framework.h"
#include <Foundation/Container/Vector.h>

using namespace sb;

namespace
{
	int instance_count = 0;

	struct A
	{
		A() { ++instance_count; }
		A(const A&) { ++instance_count; }
		~A() { --instance_count; }
		
	};
}


TEST_CASE(Vector_ConstructDestruct)
{
	A a;
	Vector<A> vec;

	for(int i=0; i < 100; ++i)
	{
		vec.PushBack(a);
	}
	ASSERT_EQUAL(instance_count, 101);

	vec.Clear();

	ASSERT_EQUAL(instance_count, 1);
}


TEST_CASE(Vector_Iterator)
{
	Vector<int> vec;
	vec.PushBack(0);
	vec.PushBack(1);
	vec.PushBack(2);
	ASSERT_EQUAL(vec.Size(), 3);

	Vector<int>::Iterator it = vec.Begin();
	ASSERT_EQUAL(*it, 0);
	++it;
	ASSERT_EQUAL(*it, 1);
	++it;
	ASSERT_EQUAL(*it, 2);
	++it;
	ASSERT_EXPR(it == vec.End());
}

TEST_CASE(Vector_Reserve)
{
	Vector<int> vec;
	vec.Reserve(32);
	ASSERT_EQUAL(vec.GetCapacity(), 32);
	ASSERT_EQUAL(vec.Size(), 0);
}

TEST_CASE(Vector_Append)
{
	Vector<int> vec;
	vec.Append(2, 32);
	ASSERT_EQUAL(vec.Size(), 32);
	ASSERT_EQUAL(vec[31], 2);
}

TEST_CASE(Vector_Insert)
{
	Vector<int> vec;
	vec.PushBack(1);
	vec.PushBack(2);
	vec.PushBack(3);

	vec.Insert(1, 1, 3);
	ASSERT_EQUAL(vec.Size(), 6);
	ASSERT_EQUAL(vec[0], 1);
	ASSERT_EQUAL(vec[1], 1);
	ASSERT_EQUAL(vec[2], 1);
	ASSERT_EQUAL(vec[3], 1);
	ASSERT_EQUAL(vec[4], 2);
	ASSERT_EQUAL(vec[5], 3);
}

TEST_CASE(Vector_Erase)
{
	Vector<int> vector;
	vector.PushBack(0);
	vector.PushBack(1);
	vector.PushBack(2);

	Vector<int>::Iterator it = vector.Begin();
	++it; // 0, >1<, 2

	it = vector.Erase(it); // 0, >2<
	vector.Erase(it);
	
	ASSERT_EQUAL(vector.Size(), 1);
	ASSERT_EQUAL(vector.Front(), 0);
}

TEST_CASE(Vector_PushBack)
{
	Vector<int> vec;
	for(int i = 0; i < 32; ++i)
	{
		vec.PushBack(1);
	}
	ASSERT_EQUAL(vec.Size(), 32);
	ASSERT_EQUAL(vec[31], 1);
}


