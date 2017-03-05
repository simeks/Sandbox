// Copyright 2008-2014 Simon Ekström

namespace sb
{

	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator::ConstIterator()
	{
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator::ConstIterator(const RedBlackTree* tree, size_t index)
		: _tree(const_cast<RedBlackTree*>(tree)), _index(index)
	{
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	const Value& RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator::operator*() const
	{
		Assert(this->_tree);
		Assert(this->_index < this->_tree->_values.Size());
		return this->_tree->_values[_index];
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	const Value* RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator::operator->() const
	{
		Assert(this->_tree);
		Assert(this->_index < this->_tree->_values.Size());

		if (!this->_tree)
			return nullptr;

		return &this->_tree->_values[_index];
	}

	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator&
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator::operator++()
	{
			Assert(this->_tree);
			Assert(this->_index < this->_tree->_values.Size());

			const Vector<NodeLinks>& node_link_array = this->_tree->_links;

			const NodeLinks& node_links = node_link_array[_index];
			size_t child_node_index = node_links.right;
			if (IsValid(child_node_index))
			{
				size_t node_index;
				do
				{
					node_index = child_node_index;
					child_node_index = node_link_array[child_node_index].left;

				} while (IsValid(child_node_index));

				this->_index = node_index;
				return *this;
			}

			size_t parent_node_index = node_links.parent;
			if (IsInvalid(parent_node_index))
			{
				SetInvalid(this->_index);
				return *this;
			}

			const NodeLinks* parent_links = &node_link_array[parent_node_index];
			child_node_index = parent_links->right;

			size_t node_index = this->_index;
			while (child_node_index == node_index)
			{
				node_index = parent_node_index;
				parent_node_index = parent_links->parent;

				if (IsInvalid(parent_node_index))
				{
					SetInvalid(this->_index);
					return *this;
				}

				parent_links = &node_link_array[parent_node_index];
				child_node_index = parent_links->right;
			}
			Assert(parent_links->left == node_index);

			this->_index = parent_node_index;

			return *this;
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator::operator++(int)
	{
			ConstIterator iterator = *this;
			++(*this);
			return iterator;
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator&
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator::operator--()
	{
			Assert(this->_tree);
			if (!this->_tree)
				return *this;

			// Allow decrementing from End() iterator
			size_t node_index = this->_index;
			if (IsInvalid(node_index))
			{
				this->_index = this->_tree->RightMostNode();
				return *this;
			}

			Assert(this->_index < this->_tree->_values.Size());
			const Vector<NodeLinks>& node_link_array = this->_tree->_links;

			const NodeLinks& node_links = node_link_array[_index];
			size_t child_node_index = node_links.left;
			if (IsValid(child_node_index))
			{
				do
				{
					node_index = child_node_index;
					child_node_index = node_link_array[child_node_index].right;

				} while (IsValid(child_node_index));

				this->_index = node_index;
				return *this;
			}

			size_t parent_node_index = node_links.parent;
			if (IsInvalid(parent_node_index))
			{
				SetInvalid(this->_index);
				return *this;
			}

			const NodeLinks* parent_links = &node_link_array[parent_node_index];
			child_node_index = parent_links->left;
			node_index = this->_index;

			while (child_node_index == node_index)
			{
				node_index = parent_node_index;
				parent_node_index = parent_links->parent;
				if (IsInvalid(parent_node_index))
				{
					SetInvalid(this->_index);
					return *this;
				}

				parent_links = &node_link_array[parent_node_index];
				child_node_index = parent_links->left;
			}
			Assert(parent_links->right == node_index);

			this->_index = parent_node_index;

			return *this;
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator::operator--(int)
	{
			ConstIterator iterator = *this;
			--(*this);
			return iterator;
		}

	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	bool RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator::operator==(const ConstIterator& other) const
	{
		return (this->_tree == other._tree && this->_index == other._index);
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	bool RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator::operator!=(const ConstIterator& other) const
	{
		return (this->_tree != other._tree || this->_index != other._index);
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	bool RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator::operator<(const ConstIterator& other) const
	{
		if (IsInvalid(this->_index))
		{
			return false;
		}
		if (IsInvalid(other._index))
		{
			return true;
		}

		if (this->_index == other._index)
		{
			return false;
		}

		Assert(this->_tree);
		Assert(other._tree);

		return this->_tree->_compare(this->_tree->_extract(this->_tree->_values[_index]),
			this->_tree->_extract(other._tree->_values[other._index]));

	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	bool RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator::operator>(const ConstIterator& other) const
	{
		if (IsInvalid(this->_index))
		{
			return false;
		}
		if (IsInvalid(other._index))
		{
			return true;
		}

		if (this->_index == other._index)
		{
			return false;
		}


		Assert(this->_tree);
		Assert(other._tree);

		return this->_tree->_compare(this->_tree->_extract(other._tree->_values[other._index]),
			this->_tree->_extract(_tree->_values[this->_index]));
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	bool RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator::operator<=(const ConstIterator& other) const
	{
		return !((*this) > other);
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	bool RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator::operator>=(const ConstIterator& other) const
	{
		return !((*this) < other);
	}

	//-------------------------------------------------------------------------------

	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator::Iterator()
	{
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator::Iterator(RedBlackTree* tree, size_t index)
		: RedBlackTree::ConstIterator(tree, index)
	{
		}

	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	Value& RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator::operator*() const
	{
		Assert(this->_tree);
		Assert(this->_index < this->_tree->_values.Size());
		return this->_tree->_values[this->_index];
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	Value* RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator::operator->() const
	{
		Assert(this->_tree);
		Assert(this->_index < this->_tree->_values.Size());
		return &this->_tree->_values[this->_index];
	}

	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator&
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator::operator++()
	{
			ConstIterator::operator++();
			return *this;
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator::operator++(int)
	{
			Iterator iterator = *this;
			++(*this);
			return iterator;
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator&
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator::operator--()
	{
			ConstIterator::operator--();
			return *this;
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator::operator--(int)
	{
			Iterator iterator = *this;
			--(*this);
			return iterator;
		}

	//-------------------------------------------------------------------------------

	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	RedBlackTree<Key, Value, CompareKey, ExtractKey>::RedBlackTree(Allocator& allocator)
		: _links(allocator),
		_values(allocator),
		_node_colors(allocator),
		_root(Invalid<size_t>())
	{
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	RedBlackTree<Key, Value, CompareKey, ExtractKey>::RedBlackTree(const RedBlackTree& other)
		: _links(other._links),
		_values(other._values),
		_node_colors(other._node_colors),
		_root(other._root)
	{
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	RedBlackTree<Key, Value, CompareKey, ExtractKey>::~RedBlackTree()
	{
		Clear();
	}

	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	size_t RedBlackTree<Key, Value, CompareKey, ExtractKey>::Size() const
	{
		return _values.Size();
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	bool RedBlackTree<Key, Value, CompareKey, ExtractKey>::Empty() const
	{
		return _values.Empty();
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	void RedBlackTree<Key, Value, CompareKey, ExtractKey>::Reserve(size_t capacity)
	{
		_values.Reserve(capacity);
		_links.Reserve(capacity);
		_node_colors.Reserve(capacity);
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	void RedBlackTree<Key, Value, CompareKey, ExtractKey>::Trim()
	{
		_values.Trim();
		_links.Trim();
		_node_colors.Trim();
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::Begin()
	{
			return Iterator(this, LeftMostNode());
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::Begin() const
	{
			return ConstIterator(this, LeftMostNode());
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::End()
	{
			return Iterator(this, Invalid<size_t>());
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::End() const
	{
			return ConstIterator(this, Invalid<size_t>());
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::Find(const Key& key)
	{
			Iterator result = LowerBound(key);
			if (result == End() || _compare(key, result->first))
				return End();
			return result;
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::Find(const Key& key) const
	{
			ConstIterator result = LowerBound(key);
			if (result == End() || _compare(key, result->first))
				return End();
			return result;
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::LowerBound(const Key& key)
	{
			// Find a leftmost node that is not less than the specified key
			return Iterator(this, LowerBoundNode(key));
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::LowerBound(const Key& key) const
	{
			// Find a leftmost node that is not less than the specified key
			return ConstIterator(this, LowerBoundNode(key));
		}

	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::UpperBound(const Key& key)
	{
			// Find a leftmost node that is greater than the specified key
			return Iterator(this, UpperBoundNode(key));
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::ConstIterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::UpperBound(const Key& key) const
	{
			// Find a leftmost node that is greater than the specified key
			return ConstIterator(this, UpperBoundNode(key));
		}


	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	Pair<typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator, bool>
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::Insert(const Value& val)
	{
			const Key& k = _extract(val);

			size_t node_index = _root;
			size_t parent_node_index = Invalid<size_t>();
			bool left_child = false;
			while (IsValid(node_index))
			{
				parent_node_index = node_index;

				const Key& node_key = _extract(_values[node_index]);
				if (_compare(k, node_key)) // Key is smaller than current node
				{
					node_index = _links[node_index].left;
					left_child = true;
				}
				else if (_compare(node_key, k)) // Key is bigger than current node
				{
					node_index = _links[node_index].right;
					left_child = false;
				}
				else
				{
					// Key found
					return Pair<Iterator, bool>(Iterator(this, node_index), false);
				}
			}

			return Pair<Iterator, bool>(InsertNodeAt(left_child, parent_node_index, val), true);
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::Insert(Iterator position, const Value& val)
	{
			Assert(position._tree == this);

			ConstIterator next;

			if (Size() == 0) // Tree is empty
			{
				return InsertNodeAt(true, _root, val);
			}

			// Check if we are able to insert the new element just next to the specified position,
			//	otherwise we have to use manual insertion without any hint, which will take longer

			if (position == Begin()) // Insert at the beginning of the tree
			{
				if (_compare(_extract(val), position->first))
				{
					return InsertNodeAt(true, position._index, val);
				}
			}
			else if (position == End()) // Insert at the end of the tree
			{
				if (_compare(_extract(_values[RightMostNode()]), _extract(val)))
				{
					return InsertNodeAt(false, RightMostNode(), val);
				}
			}
			else if (_compare(_extract(val), position->first)
				&& _compare((--(next = position))->first, _extract(val)))
			{
				// Insert between position and its predecessor
				if (IsInvalid(_links[next._index].right))
				{
					return InsertNodeAt(false, next._index, val);
				}
				else
				{
					return InsertNodeAt(true, position._index, val);
				}
			}
			else if (_compare(position->first, _extract(val))
				&& (++(next = position) == End() || _compare(_extract(val), next->first)))
			{
				// Insert between position and its successor
				if (IsInvalid(_links[position._index].right))
				{
					return InsertNodeAt(false, position._index, val);
				}
				else
				{
					return InsertNodeAt(true, next._index, val);
				}
			}

			return Insert(val).first; // Insert without position hint
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	size_t RedBlackTree<Key, Value, CompareKey, ExtractKey>::Erase(const Key& key)
	{
		Iterator result = LowerBound(key);
		if (result == End() || _compare(key, result->first))
			return 0;

		Erase(result);
		return 1;
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	void RedBlackTree<Key, Value, CompareKey, ExtractKey>::Erase(Iterator position)
	{
		Assert(position._tree == this);
		Assert(position._index < _values.Size());

		size_t remove_node_index = position._index;

		// If we only have one node
		if (_values.Size() == 1)
		{
			Assert(remove_node_index == 0);
			Assert(_root == remove_node_index);

			_values.Clear();
			_links.Clear();
			_node_colors.Clear();

			SetInvalid(_root);
			return;
		}

		NodeLinks& remove_node_links = _links[remove_node_index];
		size_t parent_node_index = remove_node_links.parent;
		size_t left_child_index = remove_node_links.left;
		size_t right_child_index = remove_node_links.right;

		// If neither child is a leaf we replace the deleted node by its predecessor
		if (IsValid(left_child_index) && IsValid(right_child_index))
		{
			// Find the predecessor
			size_t predecessor_child_index = left_child_index;
			size_t predecessor_index;
			do
			{
				predecessor_index = predecessor_child_index;
				predecessor_child_index = _links[predecessor_child_index].right;

			} while (IsValid(predecessor_child_index));

			NodeLinks& predecessor_links = _links[predecessor_index];
			size_t predecessor_parent_index = predecessor_links.parent;
			size_t predecessor_child_left_index = predecessor_links.left;
			size_t predecessor_child_right_index = predecessor_links.right;

			Assert(IsValid(predecessor_parent_index));
			NodeLinks& predecessor_parent_links = _links[predecessor_parent_index];
			bool left_child = (predecessor_parent_links.left == predecessor_index);

			// Attach the to be deleted node to the predecessors parent
			if (left_child)
			{
				Assert(predecessor_parent_links.left == predecessor_index);
				predecessor_parent_links.left = remove_node_index;
			}
			else
			{
				Assert(predecessor_parent_links.right == predecessor_index);
				predecessor_parent_links.right = remove_node_index;
			}

			// Attach any children of the predecessor to the to be deleted node
			if (IsValid(predecessor_child_left_index))
			{
				Assert(_links[predecessor_child_left_index].parent == predecessor_index);
				_links[predecessor_child_left_index].parent = remove_node_index;
			}
			if (IsValid(predecessor_child_right_index))
			{
				Assert(_links[predecessor_child_right_index].parent == predecessor_index);
				_links[predecessor_child_right_index].parent = remove_node_index;
			}

			// Attach the predecessor to the to be deleted nodes parent
			if (IsValid(parent_node_index))
			{
				Assert(_root != remove_node_index);

				NodeLinks& remove_node_parent_links = _links[parent_node_index];
				left_child = (remove_node_parent_links.left == remove_node_index);
				if (left_child)
				{
					Assert(remove_node_parent_links.left == remove_node_index);
					remove_node_parent_links.left = predecessor_index;
				}
				else
				{
					Assert(remove_node_parent_links.right == remove_node_index);
					remove_node_parent_links.right = predecessor_index;
				}
			}
			else
			{
				Assert(_root == remove_node_index);
				_root = predecessor_index;
			}

			// Attach the children to the predecessor

			Assert(_links[left_child_index].parent == remove_node_index);
			_links[left_child_index].parent = predecessor_index;

			Assert(_links[right_child_index].parent == remove_node_index);
			_links[right_child_index].parent = predecessor_index;


			// Update any changes that may have happen to the indices
			parent_node_index = predecessor_links.parent;
			left_child_index = predecessor_links.left;
			right_child_index = predecessor_links.right;

			predecessor_links = remove_node_links;

			remove_node_links.parent = parent_node_index;
			remove_node_links.left = left_child_index;
			remove_node_links.right = right_child_index;

			bool remove_node_is_black = _node_colors[remove_node_index];
			bool predecessor_is_black = _node_colors[predecessor_index];

			_node_colors[remove_node_index] = predecessor_is_black;
			_node_colors[predecessor_index] = remove_node_is_black;
		}

		Assert(IsInvalid(left_child_index) || IsInvalid(right_child_index));

		// If we are attempting to delete a black node, rebalance the tree until the node we want to delete is red
		if (_node_colors[remove_node_index]) // true = black
		{
			size_t node_index = remove_node_index;
			while (IsValid(parent_node_index))
			{
				NodeLinks& parent_links = _links[parent_node_index];
				bool left_child = (parent_links.left == node_index);

				size_t sibling_index = (left_child ? parent_links.right : parent_links.left);
				Assert(IsValid(sibling_index));
				if (!_node_colors[sibling_index]) // If sibling node is red
				{
					if (left_child)
					{
						RotateNodeRight(parent_node_index);
						Assert(parent_links.left == node_index);
						sibling_index = parent_links.right;
						Assert(IsValid(sibling_index));
					}
					else
					{
						RotateNodeLeft(parent_node_index);
						Assert(parent_links.right == node_index);
						sibling_index = parent_links.left;
						Assert(IsValid(sibling_index));
					}
				}

				{
					// Is sibling still red
					BitArray::Reference sibling_is_black = _node_colors[sibling_index];
					if (!sibling_is_black)
					{
						break;
					}

					NodeLinks& sibling_links = _links[sibling_index];
					size_t sibling_child_index = (left_child ? sibling_links.left : sibling_links.right);
					if (IsInvalid(sibling_child_index) || _node_colors[sibling_child_index])
					{
						sibling_child_index = (left_child ? sibling_links.right : sibling_links.left);
						if (IsInvalid(sibling_child_index) || _node_colors[sibling_child_index])
						{
							sibling_is_black = false;
							if (!_node_colors[parent_node_index])
							{
								_node_colors[parent_node_index] = true;
								break;
							}

							node_index = parent_node_index;
							parent_node_index = parent_links.parent;
							continue;
						}
					}
					else
					{
						sibling_child_index = (left_child ? sibling_links.right : sibling_links.left);
						if (IsInvalid(sibling_child_index) || _node_colors[sibling_child_index])
						{
							if (left_child)
							{
								sibling_child_index = sibling_links.left;
								RotateNodeLeft(sibling_index);
								sibling_index = sibling_child_index;
							}
							else
							{
								sibling_child_index = sibling_links.right;
								RotateNodeRight(sibling_index);
								sibling_index = sibling_child_index;
							}
							Assert(IsValid(sibling_index));
						}

					}
				}

				bool parent_is_black = _node_colors[parent_node_index];

				NodeLinks& sibling_links = _links[sibling_index];
				size_t sibling_child_index = (left_child ? sibling_links.right : sibling_links.left);

				Assert(IsValid(sibling_child_index));
				Assert(!_node_colors[sibling_child_index]);

				if (left_child)
				{
					RotateNodeRight(parent_node_index);
				}
				else
				{
					RotateNodeLeft(parent_node_index);
				}

				_node_colors[parent_node_index] = true;
				_node_colors[sibling_child_index] = true;
				_node_colors[sibling_index] = parent_is_black;

				break;
			}
		}

		Assert(remove_node_links.left == left_child_index);
		Assert(remove_node_links.right == right_child_index);


		// Remove the node from the tree
		size_t child_node_index = left_child_index;
		if (IsInvalid(child_node_index))
		{
			child_node_index = right_child_index;
		}

		parent_node_index = remove_node_links.parent;
		if (IsValid(parent_node_index))
		{
			Assert(_root != remove_node_index);
			NodeLinks& parent_links = _links[parent_node_index];
			if (parent_links.left == remove_node_index)
			{
				parent_links.left = child_node_index;
			}
			else
			{
				parent_links.right = child_node_index;
			}
		}
		else
		{
			Assert(_root == remove_node_index);
			_root = child_node_index;
		}

		if (IsValid(child_node_index))
		{
			_links[child_node_index].parent = parent_node_index;
		}

		// Update all indices for the node that are going to replace the removed node as we are 
		//	going to swap the removed element with the last element in our arrays.
		size_t last_node_index = _values.Size() - 1;
		if (last_node_index != remove_node_index)
		{
			NodeLinks& last_node_links = _links[last_node_index];

			// Update parent
			parent_node_index = last_node_links.parent;
			if (IsValid(parent_node_index))
			{
				Assert(_root != last_node_index);

				NodeLinks& last_node_parent_links = _links[parent_node_index];
				if (last_node_parent_links.left == last_node_index)
				{
					last_node_parent_links.left = remove_node_index;
				}
				else
				{
					Assert(last_node_parent_links.right == last_node_index);
					last_node_parent_links.right = remove_node_index;
				}
			}
			else
			{
				Assert(last_node_index == _root);
				_root = remove_node_index;
			}

			// Update children
			left_child_index = last_node_links.left;
			if (IsValid(left_child_index))
			{
				NodeLinks& child_links = _links[left_child_index];
				Assert(child_links.parent == last_node_index);
				child_links.parent = remove_node_index;
			}

			right_child_index = last_node_links.right;
			if (IsValid(last_node_links.right))
			{
				NodeLinks& child_links = _links[right_child_index];
				Assert(child_links.parent == last_node_index);
				child_links.parent = remove_node_index;

			}
		}
		_values[remove_node_index] = _values[last_node_index];
		_values.PopBack();

		_links[remove_node_index] = _links[last_node_index];
		_links.PopBack();

		_node_colors[remove_node_index] = (bool)_node_colors[last_node_index];
		_node_colors.PopBack();


		// Root node should always be black
		_node_colors[_root] = true;
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	void RedBlackTree<Key, Value, CompareKey, ExtractKey>::Clear()
	{
		_values.Clear();
		_links.Clear();
		_node_colors.Clear();

		_root = Invalid<size_t>();
	}

	//-------------------------------------------------------------------------------
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	typename RedBlackTree<Key, Value, CompareKey, ExtractKey>::Iterator
		RedBlackTree<Key, Value, CompareKey, ExtractKey>::InsertNodeAt(bool left_side, size_t where_node, const Value& val)
	{
			size_t node_index = _values.Size();
			_values.PushBack(val);
			Assert(node_index == _links.Size());
			Assert(node_index == _node_colors.Size());

			_links.PushBack(NodeLinks());
			NodeLinks& new_node_links = _links.Back();
			new_node_links.parent = where_node;
			new_node_links.left = Invalid<size_t>();
			new_node_links.right = Invalid<size_t>();

			_node_colors.PushBack(false);

			Iterator inserted = Iterator(this, node_index);

			if (IsInvalid(where_node))
			{
				Assert(_root == where_node);
				_root = node_index;
				_node_colors[node_index] = true;

				return inserted;
			}

			if (left_side)
			{
				Assert(IsInvalid(_links[where_node].left));
				_links[where_node].left = node_index;
			}
			else
			{
				Assert(IsInvalid(_links[where_node].right));
				_links[where_node].right = node_index;
			}

			node_index = where_node;
			where_node = _links[where_node].parent;
			while (IsValid(where_node))
			{
				if (_node_colors[node_index])
				{
					// No need to do any more if the node is black as there should be no 
					//	more violations in the tree.
					break;
				}

				const NodeLinks& parent_links = _links[where_node];
				size_t left_child_index = parent_links.left;
				size_t right_child_index = parent_links.right;

				bool left_child = (left_child_index == node_index);
				size_t sibling_index = (left_child ? right_child_index : left_child_index);
				Assert((left_child && (left_child_index == node_index)) || (!left_child && (right_child_index == node_index)));

				// Perform a color swap with the parent if both siblings are red.
				if (IsValid(sibling_index) && !_node_colors[sibling_index])
				{
					_node_colors[where_node] = false;
					_node_colors[sibling_index] = true;
					_node_colors[node_index] = true;
				}
				else
				{
					const NodeLinks& node_links = _links[node_index];
					size_t grand_child_index = (left_child ? node_links.left : node_links.right);

					if (IsValid(grand_child_index) && !_node_colors[grand_child_index])
					{
						if (left_child)
						{
							RotateNodeLeft(where_node);
						}
						else
						{
							RotateNodeRight(where_node);
						}
						break;
					}
					else
					{
						grand_child_index = (left_child ? node_links.right : node_links.left);
						if (IsValid(grand_child_index) && !_node_colors[grand_child_index])
						{
							if (left_child)
							{
								RotateNodeRight(node_index);
								RotateNodeLeft(where_node);
							}
							else
							{
								RotateNodeLeft(node_index);
								RotateNodeRight(where_node);
							}
							break;
						}
					}

				}
				node_index = where_node;
				where_node = parent_links.parent;
			}

			_node_colors[_root] = true;

			return inserted;
		}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	size_t RedBlackTree<Key, Value, CompareKey, ExtractKey>::LeftMostNode() const
	{
		// Traverse the tree all the way to the left to find the node with lowest sort order.

		size_t child = _root;
		size_t node = Invalid<size_t>();
		while (IsValid(child))
		{
			node = child;
			child = _links[child].left;
		}
		return node;
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	size_t RedBlackTree<Key, Value, CompareKey, ExtractKey>::RightMostNode() const
	{
		// Traverse the tree all the way to the right to find the node with highest sort order.

		size_t child = _root;
		size_t node = Invalid<size_t>();
		while (IsValid(child))
		{
			node = child;
			child = _links[child].right;
		}
		return node;
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	size_t RedBlackTree<Key, Value, CompareKey, ExtractKey>::LowerBoundNode(const Key& k) const
	{
		size_t result_index = Invalid<size_t>();
		size_t node_index = _root;
		while (IsValid(node_index))
		{
			const Key& node_key = _extract(_values[node_index]);
			if (_compare(node_key, k)) // Check if key is in the right subtree
			{
				node_index = _links[node_index].right;
			}
			else
			{
				// node_index is not less than key, so remember it
				result_index = node_index;
				node_index = _links[node_index].left;
			}
		}
		return result_index;
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	size_t RedBlackTree<Key, Value, CompareKey, ExtractKey>::UpperBoundNode(const Key& k) const
	{
		size_t result_index = Invalid<size_t>();
		size_t node_index = _root;
		while (IsValid(node_index))
		{
			const Key& node_key = _extract(_values[node_index]);
			if (_compare(k, node_key)) // Check if key is in the left subtree
			{
				// node_index is not less than key, so remember it
				result_index = node_index;
				node_index = _links[node_index].left;
			}
			else
			{
				node_index = _links[node_index].right;
			}
		}
		return result_index;
	}
	//-------------------------------------------------------------------------------
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	void RedBlackTree<Key, Value, CompareKey, ExtractKey>::RotateNodeLeft(size_t node_index)
	{
		NodeLinks& node_links = _links[node_index];
		size_t parent_index = node_links.parent;
		size_t child_index = node_links.left;
		Assert(IsValid(child_index));

		NodeLinks& child_links = _links[child_index];
		size_t grand_child_index = child_links.right;

		// Attach grandchild to node
		node_links.parent = child_index;
		node_links.left = grand_child_index;

		// Switch node with child node
		child_links.parent = parent_index;
		child_links.right = node_index;

		_node_colors[node_index] = false;
		_node_colors[child_index] = true;

		if (IsValid(parent_index))
		{
			NodeLinks& parent_links = _links[parent_index];
			if (parent_links.left == node_index)
			{
				parent_links.left = child_index;
			}
			else
			{
				Assert(parent_links.right == node_index);
				parent_links.right = child_index;
			}
		}
		else
		{
			Assert(_root == node_index);
			_root = child_index;
		}

		if (IsValid(grand_child_index))
		{
			Assert(_links[grand_child_index].parent == child_index);
			_links[grand_child_index].parent = node_index;
		}
	}
	template<typename Key, typename Value, typename CompareKey, typename ExtractKey>
	void RedBlackTree<Key, Value, CompareKey, ExtractKey>::RotateNodeRight(size_t node_index)
	{
		NodeLinks& node_links = _links[node_index];
		size_t parent_index = node_links.parent;
		size_t child_index = node_links.right;
		Assert(IsValid(child_index));

		NodeLinks& child_links = _links[child_index];
		size_t grand_child_index = child_links.left;

		// Attach grandchild to node
		node_links.parent = child_index;
		node_links.right = grand_child_index;

		// Switch node with child node
		child_links.parent = parent_index;
		child_links.left = node_index;

		_node_colors[node_index] = false;
		_node_colors[child_index] = true;

		if (IsValid(parent_index))
		{
			NodeLinks& parent_links = _links[parent_index];
			if (parent_links.left == node_index)
			{
				parent_links.left = child_index;
			}
			else
			{
				Assert(parent_links.right == node_index);
				parent_links.right = child_index;
			}
		}
		else
		{
			Assert(_root == node_index);
			_root = child_index;
		}

		if (IsValid(grand_child_index))
		{
			Assert(_links[grand_child_index].parent == child_index);
			_links[grand_child_index].parent = node_index;
		}
	}

	//-------------------------------------------------------------------------------

} // namespace sb



