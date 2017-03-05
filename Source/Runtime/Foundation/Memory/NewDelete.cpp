// Copyright 2008-2014 Simon Ekström

#include "Common.h"

using namespace sb;

void* operator new(size_t size)
{
	return memory::DefaultAllocator().Allocate(size);
}

void* operator new[](size_t size)
{
	return memory::DefaultAllocator().Allocate(size);
}

void operator delete(void* p)
{
	memory::DefaultAllocator().Free(p);
}
void operator delete[](void* p) 
{
	memory::DefaultAllocator().Free(p);
}

