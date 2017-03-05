// Copyright 2008-2014 Simon Ekström

#include "Testing/Framework.h"
#include <Foundation/Memory/LinearAllocator.h>

using namespace sb;


TEST_CASE(LinearAllocator_Allocate)
{
	LinearAllocator allocator(1024);
	void* p100 = allocator.Allocate(100);
	void* p200 = allocator.Allocate(100);

	ASSERT_NOT_EQUAL(p100, nullptr);
	ASSERT_NOT_EQUAL(p200, nullptr);
	ASSERT_EXPR(p200 > p100);

	allocator.Reset();

	void* p = allocator.Allocate(100);
	ASSERT_EQUAL(p100, p);
}


TEST_CASE(LinearAllocator_Alignment)
{
	LinearAllocator allocator(1024);
	void* p16 = allocator.Allocate(100, 16);
	void* p32 = allocator.Allocate(100, 32);
	void* p64 = allocator.Allocate(100, 64);

	ASSERT_EQUAL(p16, memory::AlignForward(p16, 16));
	ASSERT_EQUAL(p32, memory::AlignForward(p32, 32));
	ASSERT_EQUAL(p64, memory::AlignForward(p64, 64));

}
