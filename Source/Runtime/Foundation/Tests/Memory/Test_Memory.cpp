// Copyright 2008-2014 Simon Ekström

#include "Testing/Framework.h"
#include <Foundation/Memory/MemoryPool.h>

using namespace sb;


TEST_CASE(Memory_Malloc)
{
	void* p = memory::Malloc(1024);
	ASSERT_NOT_EQUAL(p, nullptr);
	ASSERT_EXPR(memory::GetAllocatedSize(p) >= 1024);
	
	p = memory::Realloc(p, 2048);
	ASSERT_NOT_EQUAL(p, nullptr);
	ASSERT_EXPR(memory::GetAllocatedSize(p) >= 2048);

	memory::Free(p);
}

TEST_CASE(Memory_AlignForward)
{
	void* p = 0x0;
	ASSERT_EQUAL(memory::AlignForward(p, 8), p);
	ASSERT_EQUAL(memory::AlignForward(p, 16), p);
	ASSERT_EQUAL(memory::AlignForward(p, 32), p);

	p = (void*)0x8;
	ASSERT_EQUAL(memory::AlignForward(p, 8), p);
	ASSERT_EQUAL(memory::AlignForward(p, 16), (void*)0x10);
	ASSERT_EQUAL(memory::AlignForward(p, 32), (void*)0x20);

	p = (void*)0x10;
	ASSERT_EQUAL(memory::AlignForward(p, 8), p);
	ASSERT_EQUAL(memory::AlignForward(p, 16), p);
	ASSERT_EQUAL(memory::AlignForward(p, 32), (void*)0x20);

	p = (void*)0x20;
	ASSERT_EQUAL(memory::AlignForward(p, 8), p);
	ASSERT_EQUAL(memory::AlignForward(p, 16), p);
	ASSERT_EQUAL(memory::AlignForward(p, 32), p);

}
