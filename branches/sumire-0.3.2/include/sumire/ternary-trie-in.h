#ifndef SUMIRE_TERNARY_TRIE_IN_H
#define SUMIRE_TERNARY_TRIE_IN_H

#include "object-io.h"
#include "ternary-trie-builder.h"

#include <cassert>

namespace sumire {

inline bool TernaryTrie::build(const TrieBase &trie)
{
	ObjectArray<TernaryTrieUnit> units;
	UInt32 num_keys = trie.num_keys();

	TernaryTrieBuilder builder;
	if (!builder.build(trie, &units))
		return false;

	clear();
	units_.swap(&units);
	num_keys_ = num_keys;

	return true;
}

inline UInt32 TernaryTrie::find_child(UInt32 index, UInt8 child_label) const
{
	assert(index < num_units());

	UInt32 left = units_[index].leftmost_child();
	UInt32 right = left + units_[index].num_branches();
	if (units_[index].has_leaf())
		++left;

	while (left < right)
	{
		UInt32 middle = (left + right) / 2;
		int comp = label(middle) - child_label;
		if (comp == 0)
			return middle;
		else if (comp < 0)
			left = middle + 1;
		else
			right = middle;
	}
	return 0;
}

inline UInt32 TernaryTrie::child(UInt32 index) const
{
	assert(index < num_units());

	if (units_[index].has_leaf())
	{
		assert(units_[index].num_branches() > 0);
		if (units_[index].num_branches() == 1)
			return 0;
	}
	else if (units_[index].num_branches() == 0)
		return 0;

	return units_[index].first_child();
}

inline UInt32 TernaryTrie::sibling(UInt32 index) const
{
	assert(index < num_units());

	if (units_[index].next_sibling() == 0)
		return 0;

	assert(index + units_[index].next_sibling() < num_units());

	return index + units_[index].next_sibling();
}

inline UInt8 TernaryTrie::label(UInt32 index) const
{
	assert(index < num_units());

	return units_[index].label();
}

inline bool TernaryTrie::get_value(UInt32 index, UInt32 *value_ptr) const
{
	assert(index < num_units());

	if (!units_[index].has_leaf())
		return false;

	if (value_ptr != NULL)
	{
		UInt32 leaf_index = units_[index].leftmost_child();

		assert(leaf_index < num_units());
		assert(units_[leaf_index].label() == '\0');

		*value_ptr = units_[leaf_index].value();
	}
	return true;
}

inline UInt32 TernaryTrie::num_nodes() const
{
	assert(num_keys() <= num_units());

	return num_units() - num_keys();
}

inline void TernaryTrie::clear()
{
	units_.clear();
	num_keys_ = 0;
}

inline void *TernaryTrie::map(void *addr)
{
	assert(addr != NULL);

	clear();

	addr = units_.map(addr);

	ObjectClipper clipper(addr);
	num_keys_ = *clipper.clip<UInt32>();
	addr = clipper.addr();

	return addr;
}

inline bool TernaryTrie::read(std::istream *input)
{
	assert(input != NULL);

	ObjectArray<TernaryTrieUnit> units;

	if (!units.read(input))
		return false;

	ObjectReader reader(input);

	UInt32 num_keys;
	if (!reader.read(&num_keys))
		return false;

	clear();
	units_.swap(&units);
	num_keys_ = num_keys;

	return true;
}

inline bool TernaryTrie::write(std::ostream *output) const
{
	assert(output != NULL);

	if (!units_.write(output))
		return false;

	ObjectWriter writer(output);
	if (!writer.write(num_keys_))
		return false;

	return true;
}

inline void TernaryTrie::swap(TernaryTrie *target)
{
	assert(target != NULL);

	units_.swap(&target->units_);
	std::swap(num_keys_, target->num_keys_);
}

}  // namespace sumire

#endif  // SUMIRE_TERNARY_TRIE_IN_H
