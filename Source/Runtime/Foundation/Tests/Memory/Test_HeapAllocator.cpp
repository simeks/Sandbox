// Copyright 2008-2014 Simon Ekström

#include "Testing/Framework.h"
#include <Foundation/Memory/HeapAllocator.h>

using namespace sb;


TEST_CASE(HeapAllocator_Allocate)
{
	HeapAllocator allocator;
	void* p = allocator.Allocate(100);

	ASSERT_NOT_EQUAL(p, nullptr);
	ASSERT_EXPR(allocator.GetAllocatedSize(p) >= 100);

	allocator.Free(p);
}

TEST_CASE(HeapAllocator_Alignment)
{
	HeapAllocator allocator;
	void* p16 = allocator.Allocate(100, 16);
	void* p32 = allocator.Allocate(100, 32);
	void* p64 = allocator.Allocate(100, 64);

	ASSERT_EQUAL(p16, memory::AlignForward(p16, 16));
	ASSERT_EQUAL(p32, memory::AlignForward(p32, 32));
	ASSERT_EQUAL(p64, memory::AlignForward(p64, 64));

	allocator.Free(p16);
	allocator.Free(p32);
	allocator.Free(p64);
}

TEST_CASE(HeapAllocator_NewDelete)
{
	HeapAllocator allocator;

	struct Obj
	{
		int x, y, z;
		bool* p;

		Obj(int a, bool* ptr) : x(a), y(2), z(3), p(ptr) { *p = true; }
		~Obj() { *p = false; }

	}* obj;

	bool alive = false;
	obj = SB_NEW(allocator, Obj, 1, &alive);
	ASSERT_NOT_EQUAL(obj, nullptr);

	ASSERT_EQUAL(obj->x, 1);
	ASSERT_EQUAL(obj->y, 2);
	ASSERT_EQUAL(obj->z, 3);
	ASSERT_EQUAL(alive, true);

	SB_DELETE(allocator, obj);

	ASSERT_EQUAL(alive, false);
}



