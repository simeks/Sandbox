// Copyright 2008-2014 Simon Ekström

#include "Testing/Framework.h"
#include <Foundation/Memory/MemoryPool.h>

using namespace sb;


TEST_CASE(MemoryPool_Allocate)
{
	struct Obj
	{
		int a;

		Obj() : a(1) { }
	};

	MemoryPool<Obj, 4> pool;
	Obj* obj = new (pool.Allocate()) Obj();
	ASSERT_NOT_EQUAL(obj, nullptr);
	ASSERT_EQUAL(obj->a, 1);

	obj->a = 2;
	uint32_t x = pool.GetIndex(obj);
	ASSERT_EQUAL(x, 0);

	Obj* objs[128];
	for (int i = 0; i < 128; ++i)
	{
		objs[i] = new (pool.Allocate()) Obj();
		ASSERT_NOT_EQUAL(objs[i], nullptr);
		int j = pool.GetIndex(objs[i]);
		ASSERT_EQUAL(pool.GetObject(j), objs[i]);
	}

	obj = pool.GetObject(x);
	ASSERT_EQUAL(obj->a, 2);

	for (int i = 0; i < 128; ++i)
	{
		pool.Release(objs[i]);
	}

	obj = pool.GetObject(x);
	ASSERT_EQUAL(obj->a, 2);

	for (int i = 0; i < 128; ++i)
	{
		objs[i] = new (pool.Allocate()) Obj();
		ASSERT_NOT_EQUAL(objs[i], nullptr);
		int j = pool.GetIndex(objs[i]);
		ASSERT_EQUAL(pool.GetObject(j), objs[i]);
	}
}

