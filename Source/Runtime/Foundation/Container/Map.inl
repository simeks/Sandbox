// Copyright 2008-2014 Simon Ekström


namespace sb
{

	//-------------------------------------------------------------------------------
	template<typename Key, typename Value, typename Compare>
	Map<Key, Value, Compare>::Map(Allocator& allocator)
		: BaseClass(allocator)
	{
	}
	template<typename Key, typename Value, typename Compare>
	Map<Key, Value, Compare>::Map(const Map& source)
		: BaseClass(source)
	{

	}
	template<typename Key, typename Value, typename Compare>
	Map<Key, Value, Compare>::~Map()
	{
	}

	//-------------------------------------------------------------------------------
	template<typename Key, typename Value, typename Compare>
	Value& Map<Key, Value, Compare>::operator[](const Key& key)
	{
		Compare comp;
		typename BaseClass::Iterator result = BaseClass::LowerBound(key);
		if (result == BaseClass::End() || comp(key, result->first))
		{
			// The key doesn't exist in the tree so we insert it
			result = BaseClass::Insert(result, Pair<Key, Value>(key, Value()));
		}

		return result->second;
	}
	//-------------------------------------------------------------------------------

} // namespace sb


