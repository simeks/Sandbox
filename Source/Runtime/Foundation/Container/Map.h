// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_MAP_H__
#define __FOUNDATION_MAP_H__

#include "RedBlackTree.h"

namespace sb
{

	/// Map container implemented as a red-black tree.
	template<typename Key, typename Value, typename Compare = Less<Key>>
	class Map : public RedBlackTree<Key, Pair<Key, Value>, Compare, SelectFirst<Pair<Key, Value>>>
	{
		typedef RedBlackTree<Key, Pair<Key, Value>, Compare, SelectFirst<Pair<Key, Value>>> BaseClass;

	public:
		Map(Allocator& allocator = memory::DefaultAllocator());
		Map(const Map& source);
		~Map();

		Value& operator[](const Key& key);

	};

} // namespace sb



#include "Map.inl"


#endif // __FOUNDATION_MAP_H__

