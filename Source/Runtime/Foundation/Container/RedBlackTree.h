// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_RBTREE_H__
#define __FOUNDATION_RBTREE_H__

#include "Pair.h"
#include "BitArray.h"


namespace sb
{

	/// Red-black tree implementation
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	class RedBlackTree
	{
	public:
		class ConstIterator
		{
			friend RedBlackTree;
		public:
			ConstIterator();
			ConstIterator(const RedBlackTree* tree, size_t index);

			const Value& operator*() const;
			const Value* operator->() const;

			ConstIterator& operator++();
			ConstIterator operator++(int);
			ConstIterator& operator--();
			ConstIterator operator--(int);

			bool operator==(const ConstIterator& other) const;
			bool operator!=(const ConstIterator& other) const;
			bool operator<(const ConstIterator& other) const;
			bool operator>(const ConstIterator& other) const;
			bool operator<=(const ConstIterator& other) const;
			bool operator>=(const ConstIterator& other) const;

		protected:
			RedBlackTree* _tree;
			size_t _index;
		};

		class Iterator : public ConstIterator
		{
			friend RedBlackTree;
		public:
			Iterator();
			Iterator(RedBlackTree* tree, size_t index);

			Value& operator*() const;
			Value* operator->() const;

			Iterator& operator++();
			Iterator operator++(int);
			Iterator& operator--();
			Iterator operator--(int);
		};


	public:
		RedBlackTree(Allocator& allocator = memory::DefaultAllocator());
		RedBlackTree(const RedBlackTree& other);
		~RedBlackTree();

		/// @brief Returns the number of elements in the tree
		size_t Size() const;

		/// @brief Returns true if the tree is empty
		bool Empty() const;

		/// @brief Increases the capacity of the tree
		void Reserve(size_t capacity);

		/// @brief Trims the tree capacity to match its size
		void Trim();



		/// @brief Returns an iterator to the beginning of this tree
		Iterator Begin();

		/// @brief Returns an const iterator to the beginning of this tree
		ConstIterator Begin() const;

		/// @brief Returns an iterator to the end of this tree
		Iterator End();

		/// @brief Returns an const iterator to the end of this tree
		ConstIterator End() const;


		/// @brief Find a node in the tree with the specified key
		Iterator Find(const Key& key);
		/// @brief Find a node in the tree with the specified key
		ConstIterator Find(const Key& key) const;

		/// @brief Find a leftmost node that is not less than the specified key
		Iterator LowerBound(const Key& key);
		/// @brief Find a leftmost node that is not less than the specified key
		ConstIterator LowerBound(const Key& key) const;

		/// @brief Find a leftmost node that is greater than the specified key
		Iterator UpperBound(const Key& key);
		/// @brief Find a leftmost node that is greated than the specified key
		ConstIterator UpperBound(const Key& key) const;


		/// @name Modifiers
		/// @{

		/// @brief Inserts a new element to the tree
		/// @return A pair containing an iterator and a boolean value. If the value was inserted,
		///			the boolean will be true and the iterator will reference the inserted node.
		///			If a node with the same key already exists, then the boolean will be false and
		///			the iterator will reference the existing node.
		Pair<Iterator, bool> Insert(const Value& val);

		/// @brief Inserts a new element at the specified position
		/// @return Iterator referencing either the new node if it was inserted, or an existing
		///			node if there already was a node with the specified key.
		Iterator Insert(Iterator position, const Value& val);

		/// @brief Removes an element with the specified key from the tree
		/// @return The number of elements erased
		size_t Erase(const Key& key);

		/// @brief Removes an element at the specified position
		void Erase(Iterator position);

		/// @brief Removes all items from the tree
		void Clear();

		/// @}


	protected:
		CompareKey _compare;
		ExtractKey _extract;

		/// Node link data
		struct NodeLinks
		{
			size_t parent;	///< Parent node
			size_t left;	///< Left child
			size_t right;	///< Right child
		};

		Vector<NodeLinks> _links; ///< Node links
		Vector<Value> _values; ///< Values for each node
		BitArray _node_colors; ///< Color of each node, 0 = red, 1 = black

		size_t _root; ///< Root node


		/// Inserts a node with the specified value at the specified position
		/// @param left_side Which side should the new node be placed
		/// @param where_node Which node should the new node be attached to
		/// @param val Value for the new node
		Iterator InsertNodeAt(bool left_side, size_t where_node, const Value& val);

		/// Returns the index of the leftmost node in the tree, the node with lowest sort order.
		size_t LeftMostNode() const;
		/// Returns the index of the rightmost node in the tree, the node with highest sort order.
		size_t RightMostNode() const;

		/// Returns the lower bound node for the specified key
		size_t LowerBoundNode(const Key& k) const;
		/// Returns the upper bound node for the specified key
		size_t UpperBoundNode(const Key& k) const;


		/// Rotates a node with its left child
		void RotateNodeLeft(size_t node_index);
		/// Rotates a node with its right child
		void RotateNodeRight(size_t node_index);

#ifdef SANDBOX_BUILD_DEBUG
		/// Verifies if the specified node is valid, also verifies all children recursively
		/// @return	Number of black child nodes, INVALID_INDEX at error
		uint32_t RecursiveVerify(size_t node_index);
#endif 

	public:
		INLINE Iterator begin()
		{
			return Begin();
		}
		INLINE ConstIterator begin() const
		{
			return Begin();
		}
		INLINE Iterator end()
		{
			return End();
		}
		INLINE ConstIterator end() const
		{
			return End();
		}
	};

} // namespace sb



#include "RedBlackTree.inl"

#endif // __FOUNDATION_RBTREE_H__


