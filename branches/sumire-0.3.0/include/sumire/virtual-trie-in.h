#ifndef SUMIRE_VIRTUAL_TRIE_IN_H
#define SUMIRE_VIRTUAL_TRIE_IN_H

#include "object-io.h"

#include <cassert>

namespace sumire {

inline VirtualTrie::VirtualTrie(const ObjectPool<BasicTrieUnit> &unit_pool,
	const ObjectPool<UInt8> &label_pool, UInt32 num_keys)
	: unit_pool_(&unit_pool), label_pool_(&label_pool), num_keys_(num_keys) {}

inline UInt32 VirtualTrie::find_child(UInt32 index, UInt8 child_label) const
{
	for (UInt32 child_index = child(index); child_index != 0;
		child_index = sibling(child_index))
	{
		if (label(child_index) == child_label)
			return child_index;
	}
	return 0;
}

inline UInt32 VirtualTrie::child(UInt32 index) const
{
	assert(index < num_units());

	UInt32 child_index = (*unit_pool_)[index].child();
	if (child_index == 0)
		return 0;

	assert(child_index < num_units());

	if ((*label_pool_)[child_index] == '\0')
	{
		if (!(*unit_pool_)[child_index].has_sibling())
			return 0;
		++child_index;
	}

	assert(child_index < num_units());

	return child_index;
}

inline UInt32 VirtualTrie::sibling(UInt32 index) const
{
	assert(index < num_units());

	return (*unit_pool_)[index].has_sibling() ? (index + 1) : 0;
}

inline UInt8 VirtualTrie::label(UInt32 index) const
{
	assert(index < num_units());

	return (*label_pool_)[index];
}

inline bool VirtualTrie::get_value(UInt32 index, UInt32 *value_ptr) const
{
	assert(index < num_units());

	UInt32 leaf_index = (*unit_pool_)[index].child();
	if (leaf_index == 0)
		return false;

	assert(leaf_index < num_units());

	if ((*label_pool_)[leaf_index] != '\0')
		return false;

	if (value_ptr != NULL)
		*value_ptr = (*unit_pool_)[leaf_index].value();
	return true;
}

inline UInt32 VirtualTrie::num_nodes() const
{
	assert(num_keys() <= num_units());

	return num_units() - num_keys();
}

inline void VirtualTrie::clear()
{
	unit_pool_ = NULL;
	label_pool_ = NULL;
	num_keys_ = 0;
}

inline bool VirtualTrie::write(std::ostream *output) const
{
	assert(output != NULL);

	if (!unit_pool_->write(output))
		return false;
	if (!label_pool_->write(output))
		return false;

	ObjectWriter writer(output);
	if (!writer.write(num_keys_))
		return false;

	return true;
}

inline void VirtualTrie::swap(VirtualTrie *target)
{
	std::swap(unit_pool_, target->unit_pool_);
	std::swap(label_pool_, target->label_pool_);
	std::swap(num_keys_, target->num_keys_);
}

}  // namespace sumire

#endif  // SUMIRE_VIRTUAL_TRIE_IN_H
