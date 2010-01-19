#ifndef SUMIRE_BASIC_TRIE_IN_H
#define SUMIRE_BASIC_TRIE_IN_H

#include "basic-trie-builder.h"
#include "object-io.h"

#include <cassert>

namespace sumire {

inline bool BasicTrie::build(const TrieBase &trie, Order order)
{
	ObjectArray<BasicTrieUnit> units;
	ObjectArray<UInt8> labels;
	UInt32 num_keys = trie.num_keys();

	BasicTrieBuilder builder;
	switch (order)
	{
	case LEVEL_ORDER:
		if (!builder.build_in_level_order(trie, &units, &labels))
			return false;
		break;
	case BREADTH_ORDER:
		if (!builder.build_in_breadth_order(trie, &units, &labels))
			return false;
		break;
	case MAX_VALUE_ORDER:
		if (!builder.build_in_max_value_order(trie, &units, &labels))
			return false;
		break;
	case TOTAL_VALUE_ORDER:
		if (!builder.build_in_total_value_order(trie, &units, &labels))
			return false;
		break;
	}

	clear();
	units_.swap(&units);
	labels_.swap(&labels);
	num_keys_ = num_keys;

	return true;
}

inline bool BasicTrie::build(const TrieBase &trie)
{
	ObjectArray<BasicTrieUnit> units;
	ObjectArray<UInt8> labels;
	UInt32 num_keys = trie.num_keys();

	BasicTrieBuilder builder;
	if (!builder.build(trie, &units, &labels))
		return false;

	clear();
	units_.swap(&units);
	labels_.swap(&labels);
	num_keys_ = num_keys;

	return true;
}

inline UInt32 BasicTrie::find_child(UInt32 index, UInt8 child_label) const
{
	for (UInt32 child_index = child(index); child_index != 0;
		child_index = sibling(child_index))
	{
		if (label(child_index) == child_label)
			return child_index;
	}
	return 0;
}

inline UInt32 BasicTrie::child(UInt32 index) const
{
	assert(index < num_units());

	UInt32 child_index = units_[index].child();
	if (child_index == 0)
		return 0;

	assert(child_index < num_units());

	if (labels_[child_index] == '\0')
	{
		if (!units_[child_index].has_sibling())
			return 0;
		++child_index;
	}

	assert(child_index < num_units());

	return child_index;
}

inline UInt32 BasicTrie::sibling(UInt32 index) const
{
	assert(index < num_units());

	return units_[index].has_sibling() ? (index + 1) : 0;
}

inline UInt8 BasicTrie::label(UInt32 index) const
{
	assert(index < num_units());

	return labels_[index];
}

inline bool BasicTrie::get_value(UInt32 index, UInt32 *value_ptr) const
{
	assert(index < num_units());

	UInt32 leaf_index = units_[index].child();
	if (leaf_index == 0)
		return false;

	assert(leaf_index < num_units());

	if (labels_[leaf_index] != '\0')
		return false;

	if (value_ptr != NULL)
		*value_ptr = units_[leaf_index].value();
	return true;
}

inline UInt32 BasicTrie::num_nodes() const
{
	assert(num_keys() <= num_units());

	return num_units() - num_keys();
}

inline void BasicTrie::clear()
{
	units_.clear();
	labels_.clear();
	num_keys_ = 0;
}

inline void *BasicTrie::map(void *addr)
{
	assert(addr != NULL);

	clear();

	addr = units_.map(addr);
	addr = labels_.map(addr);

	ObjectClipper clipper(addr);
	num_keys_ = *clipper.clip<UInt32>();
	addr = clipper.addr();

	return addr;
}

inline bool BasicTrie::read(std::istream *input)
{
	assert(input != NULL);

	ObjectArray<BasicTrieUnit> units;
	ObjectArray<UInt8> labels;

	if (!units.read(input))
		return false;
	if (!labels.read(input))
		return false;

	ObjectReader reader(input);

	UInt32 num_keys;
	if (!reader.read(&num_keys))
		return false;

	clear();
	units_.swap(&units);
	labels_.swap(&labels);
	num_keys_ = num_keys;

	return true;
}

inline bool BasicTrie::write(std::ostream *output) const
{
	assert(output != NULL);

	if (!units_.write(output))
		return false;
	if (!labels_.write(output))
		return false;

	ObjectWriter writer(output);
	if (!writer.write(num_keys_))
		return false;

	return true;
}

inline void BasicTrie::swap(BasicTrie *target)
{
	assert(target != NULL);

	units_.swap(&target->units_);
	labels_.swap(&target->labels_);
	std::swap(num_keys_, target->num_keys_);
}

}  // namespace sumire

#endif  // SUMIRE_BASIC_TRIE_IN_H
