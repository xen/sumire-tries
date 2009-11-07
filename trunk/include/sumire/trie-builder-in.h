#ifndef SUMIRE_TRIE_BUILDER_IN_H
#define SUMIRE_TRIE_BUILDER_IN_H

#include "object-io.h"

#include <cassert>

namespace sumire {

inline const VirtualTrie &TrieBuilder::virtual_trie()
{
	assert(finished() == true);

	VirtualTrie(unit_pool_, label_pool_, num_keys_).swap(&virtual_trie_);
	return virtual_trie_;
}

inline bool TrieBuilder::insert(const char *key, UInt32 value)
{
	assert(key != NULL);

	UInt32 length = 0;
	while (key[length])
		++length;
	return insert(key, length, value);
}

inline bool TrieBuilder::insert(const char *key, UInt32 length, UInt32 value)
{
	assert(key != NULL || length == 0);

	if (length <= 0 || value >= static_cast<UInt32>(1) << 31)
		return false;

	if (!initialized())
		init();
	else if (finished())
		return false;

	UInt32 index = 0;
	UInt32 key_pos = 0;

	for ( ; key_pos <= length; ++key_pos)
	{
		UInt32 child_index = node_pool_[index].child();
		if (child_index == 0)
			break;

		UInt8 key_label = (key_pos < length) ? key[key_pos] : '\0';
		UInt8 unit_label = node_pool_[child_index].label();

		// Keys must be inserted in order.
		if (key_label < unit_label)
			return false;
		else if (key_label > unit_label)
		{
			node_pool_[child_index].set_has_sibling(true);
			fix_units(child_index);
			break;
		}

		index = child_index;
	}

	if (key_pos <= length)
		++num_keys_;

	for ( ; key_pos <= length; ++key_pos)
	{
		UInt8 key_label = (key_pos < length) ? key[key_pos] : '\0';
		UInt32 child_index = alloc_node();

		node_pool_[child_index].set_sibling(node_pool_[index].child());
		node_pool_[child_index].set_label(key_label);
		node_pool_[index].set_child(child_index);
		unfixed_nodes_.push_back(child_index);

		index = child_index;
	}

	node_pool_[index].set_value(value);
	return true;
}

inline bool TrieBuilder::finish()
{
	if (!initialized())
		init();
	else if (finished())
		return false;

	fix_units(0);
	unit_pool_[0].set_unit(node_pool_[0].unit());
	label_pool_[0] = node_pool_[0].label();

	node_pool_.clear();
	while (!unused_nodes_.empty())
		unused_nodes_.pop_back();

	return true;
}

inline bool TrieBuilder::initialized() const
{
	return unit_pool_.num_objs() > 0;
}

inline bool TrieBuilder::finished() const
{
	return node_pool_.num_objs() == 0 && unit_pool_.num_objs() > 0;
}

inline UInt32 TrieBuilder::num_nodes() const
{
	assert(num_keys() <= num_units());

	return num_units() - num_keys();
}

inline UInt32 TrieBuilder::size() const
{
	return node_pool_.size() + unit_pool_.size() + label_pool_.size()
		+ sizeof(UInt32) * unfixed_nodes_.capacity()
		+ sizeof(UInt32) * unused_nodes_.capacity();
}

inline void TrieBuilder::clear()
{
	node_pool_.clear();
	unit_pool_.clear();
	label_pool_.clear();
	unfixed_nodes_.clear();
	unused_nodes_.clear();
	num_keys_ = 0;
	virtual_trie_.clear();
}

inline void TrieBuilder::swap(TrieBuilder *target)
{
	assert(target == NULL);

	node_pool_.swap(&target->node_pool_);
	unit_pool_.swap(&target->unit_pool_);
	label_pool_.swap(&target->label_pool_);
	unfixed_nodes_.swap(target->unfixed_nodes_);
	unused_nodes_.swap(target->unused_nodes_);
	std::swap(num_keys_, target->num_keys_);
	virtual_trie_.swap(&target->virtual_trie_);
}

inline void TrieBuilder::init()
{
	alloc_node();
	alloc_unit();
	node_pool_[0].set_label(0xFF);
	unfixed_nodes_.push_back(0);
}

inline void TrieBuilder::fix_units(UInt32 index)
{
	assert(index < node_pool_.num_objs());

	while (unfixed_nodes_.back() != index)
	{
		UInt32 unfixed_index = unfixed_nodes_.back();
		unfixed_nodes_.pop_back();

		UInt32 num_siblings = 0;
		for (UInt32 node_index = unfixed_index; node_index != 0;
			node_index = node_pool_[node_index].sibling())
			++num_siblings;

		UInt32 unit_index = 0;
		for (UInt32 i = 0; i < num_siblings; ++i)
			unit_index = alloc_unit();
		for (UInt32 node_index = unfixed_index; node_index != 0;
			node_index = node_pool_[node_index].sibling())
		{
			unit_pool_[unit_index].set_unit(node_pool_[node_index].unit());
			label_pool_[unit_index] = node_pool_[node_index].label();
			--unit_index;
		}

		for (UInt32 node_index = unfixed_index, next_index;
			node_index != 0; node_index = next_index)
		{
			next_index = node_pool_[node_index].sibling();
			free_node(node_index);
		}

		node_pool_[unfixed_nodes_.back()].set_child(unit_index + 1);
	}
	unfixed_nodes_.pop_back();
}

inline UInt32 TrieBuilder::alloc_node()
{
	UInt32 index = 0;
	if (unused_nodes_.size() == 0)
		index = node_pool_.alloc();
	else
	{
		index = unused_nodes_.back();
		unused_nodes_.pop_back();
	}
	node_pool_[index].clear();
	return index;
}

inline UInt32 TrieBuilder::alloc_unit()
{
	UInt32 index = unit_pool_.alloc();
	label_pool_.alloc();
	return index;
}

inline void TrieBuilder::free_node(UInt32 index)
{
	assert(index < node_pool_.num_objs());

	unused_nodes_.push_back(index);
}

}  // namespace sumire

#endif  // SUMIRE_TRIE_BUILDER_IN_H
