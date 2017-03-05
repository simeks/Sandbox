// Copyright 2008-2014 Simon Ekström

#include "Testing/Framework.h"

#include <Foundation/Container/Deque.h>

using namespace sb;

static int count = 0;
struct Element
{
	Element() { ++count; } 
	Element(const Element&) { ++count; }
	~Element() { --count; }

};


TEST_CASE(Deque_Object)
{
	Element elem;
	{
		Deque<Element> deq;
		deq.PushBack(elem);
		deq.PopFront();
	}
	ASSERT_EQUAL(count, 1);

}

TEST_CASE(Deque_Push)
{
	Deque<int> queue;
	queue.PushBack(0);
	queue.PushBack(1);
	queue.PushFront(2);

	// Expected: 2 0 1
	
	ASSERT_EQUAL(queue.Back(), 1);
	ASSERT_EQUAL(queue.Front(), 2);
}

TEST_CASE(Deque_Pop)
{
	Deque<int> queue;
	queue.PushBack(0);
	queue.PushBack(1);
	queue.PushFront(2);

	queue.PopBack();
	queue.PopFront();

	ASSERT_EQUAL(queue.Back(), 0);
	ASSERT_EQUAL(queue.Front(), 0);
}




TEST_CASE(Deque_Insert)
{
	Deque<int> queue;
	queue.PushBack(0);
	queue.PushBack(1);
	
	Deque<int>::Iterator it = queue.Begin(); 
	++it; // 0, >1<

	it = queue.Insert(it, 2); // 0, >2<, 1
	ASSERT_EQUAL(*it, 2);

	it = queue.Insert(it, 3); // 0, >3<, 2, 1
	ASSERT_EQUAL(*it, 3);

	it = queue.Begin();
	ASSERT_EQUAL(*it, 0);
	++it;
	ASSERT_EQUAL(*it, 3);
	++it;
	ASSERT_EQUAL(*it, 2);
	++it;
	ASSERT_EQUAL(*it, 1);
}

TEST_CASE(Deque_Erase)
{
	Deque<int> queue;
	queue.PushBack(0);
	queue.PushBack(1);
	queue.PushBack(2);

	Deque<int>::Iterator it = queue.Begin();
	++it; // 0, >1<, 2

	it = queue.Erase(it); // 0, >2<
	queue.Erase(it);
	
	ASSERT_EQUAL(queue.Size(), 1);
	ASSERT_EQUAL(queue.Front(), 0);
}

TEST_CASE(Deque_Iterator)
{
	Deque<int> queue;
	queue.PushBack(0);
	queue.PushBack(1);
	queue.PushBack(2);
	ASSERT_EQUAL(queue.Size(), 3);

	Deque<int>::Iterator it = queue.Begin();
	ASSERT_EQUAL(*it, 0);
	++it;
	ASSERT_EQUAL(*it, 1);
	++it;
	ASSERT_EQUAL(*it, 2);
	++it;
	ASSERT_EXPR(it == queue.End());
}

TEST_CASE(Deque_Size)
{
	Deque<int> queue;
	ASSERT_EQUAL(queue.Size(), 0);
	ASSERT_EXPR(queue.Empty());

	queue.PushBack(0);
	queue.PushBack(1);
	
	ASSERT_EQUAL(queue.Size(), 2);
	ASSERT_EXPR(!queue.Empty());
}

TEST_CASE(Deque_Clear)
{
	Deque<int> queue;

	queue.PushBack(0);
	queue.PushBack(1);

	queue.Clear();
	ASSERT_EXPR(queue.Empty());
}

TEST_CASE(Deque_Copy)
{
	Deque<int> queue;

	queue.PushBack(0);
	queue.PushBack(1);

	Deque<int> queue_copy(queue);

	ASSERT_EQUAL(queue_copy.Size(), 2);
	ASSERT_EQUAL(queue_copy.Front(), 0);
	ASSERT_EQUAL(queue_copy.Back(), 1);
}

TEST_CASE(Deque_Construct)
{
	Deque<int> queue;

	queue.PushBack(0);
	queue.PushBack(1);

	Deque<int> queue_copy;
	queue_copy = queue;

	ASSERT_EQUAL(queue_copy.Size(), 2);
	ASSERT_EQUAL(queue_copy.Front(), 0);
	ASSERT_EQUAL(queue_copy.Back(), 1);
}

TEST_CASE(Deque_Reserve)
{
	Deque<int> queue;
	queue.Reserve(32);
	ASSERT_EQUAL(queue.GetCapacity(), 32);
	ASSERT_EQUAL(queue.Size(), 0);
}
