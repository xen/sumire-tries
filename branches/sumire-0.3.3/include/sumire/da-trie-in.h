#ifndef SUMIRE_DA_TRIE_IN_H
#define SUMIRE_DA_TRIE_IN_H

#include "da-trie-builder.h"
#include "object-io.h"

#include <cassert>

namespace sumire {

inline bool DaTrie::build(const TrieBase &trie)
{
	ObjectArray<DaTrieUnit> units;
	UInt32 num_nodes = trie.num_nodes();
	UInt32 num_keys = trie.num_keys();

	DaTrieBuilder builder;
	if (!builder.build(trie, &units))
		return false;

	clear();
	units_.swap(&units);
	num_nodes_ = num_nodes;
	num_keys_ = num_keys;

	return true;
}

inline UInt32 DaTrie::find_child(UInt32 index, UInt8 child_label) const
{
	assert(index < num_units());

	if (child_label == '\0')
		return 0;

	index = units_[index].offset() ^ child_label;
	if (units_[index].label() == child_label)
		return index;
	return 0;
}

inline UInt32 DaTrie::child(UInt32 index) const
{
	assert(index < num_units());

	if (units_[index].child_label() == '\0')
		return 0;

	return units_[index].offset() ^ units_[index].child_label();
}

inline UInt32 DaTrie::sibling(UInt32 index) const
{
	assert(index < num_units());

	if (units_[index].next_sibling() == 0)
		return 0;

	return index ^ units_[index].next_sibling();
}

inline UInt8 DaTrie::label(UInt32 index) const
{
	assert(index < num_units());

	return units_[index].label();
}

inline bool DaTrie::get_value(UInt32 index, UInt32 *value_ptr) const
{
	assert(index < num_units());

	if (!units_[index].has_leaf())
		return false;

	if (value_ptr != NULL)
	{
		UInt32 leaf_index = units_[index].offset();
		assert(leaf_index < num_units());

		*value_ptr = units_[leaf_index].value();
	}

	return true;
}

inline void DaTrie::clear()
{
	units_.clear();
	num_nodes_ = 0;
	num_keys_ = 0;
}

inline void *DaTrie::map(void *addr)
{
	assert(addr != NULL);

	addr = units_.map(addr);

	ObjectClipper clipper(addr);
	num_nodes_ = *clipper.clip<UInt32>();
	num_keys_ = *clipper.clip<UInt32>();

	return clipper.addr();
}

inline bool DaTrie::read(std::istream *input)
{
	assert(input != NULL);

	ObjectArray<DaTrieUnit> units;
	if (!units.read(input))
		return false;

	ObjectReader reader(input);

	UInt32 num_nodes, num_keys;
	if (!reader.read(&num_nodes) || !reader.read(&num_keys))
		return false;

	clear();
	units_.swap(&units);
	num_nodes_ = num_nodes;
	num_keys_ = num_keys;

	return true;
}

inline bool DaTrie::write(std::ostream *output) const
{
	assert(output != NULL);

	if (!units_.write(output))
		return false;

	ObjectWriter writer(output);
	if (!writer.write(num_nodes_) || !writer.write(num_keys_))
		return false;

	return true;
}

inline void DaTrie::swap(DaTrie *target)
{
	units_.swap(&target->units_);
	std::swap(num_nodes_, target->num_nodes_);
	std::swap(num_keys_, target->num_keys_);
}

}  // namespace sumire

#endif  // SUMIRE_DA_TRIE_IN_H
