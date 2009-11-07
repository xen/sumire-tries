#ifndef SUMIRE_TERNARY_TRIE_BUILDER_IN_H
#define SUMIRE_TERNARY_TRIE_BUILDER_IN_H

#include <algorithm>
#include <cassert>
#include <vector>

namespace sumire {

inline bool TernaryTrieBuilder::build(const TrieBase &trie,
	ObjectArray<TernaryTrieUnit> *units)
{
	assert(units != NULL);

	if (trie.num_nodes() == 0)
		return false;

	clear();
	trie_ = &trie;
	units_.resize(trie.num_nodes() + trie.num_keys());
	++num_units_;

	units_[0].set_label(trie_->label(trie_->root()));
	build_trie(trie_->root(), 0);

	assert(num_units_ == units_.num_objs());

	build_link();

	units_.swap(units);
	clear();

	return true;
}

inline void TernaryTrieBuilder::build_trie(
	UInt32 src_index, UInt32 dest_index)
{
	assert(src_index < trie_->num_units());
	assert(dest_index < num_units_);
	assert(trie_->label(src_index) != '\0');

	units_[dest_index].set_leftmost_child(num_units_);

	UInt32 value;
	if (trie_->get_value(src_index, &value))
	{
		units_[dest_index].set_has_leaf();
		units_[num_units_++].set_value(value);
	}

	UInt32 src_child_index = trie_->child(src_index);
	UInt32 dest_child_index = num_units_;
	while (src_child_index != 0)
	{
		units_[num_units_++].set_label(trie_->label(src_child_index));

		src_child_index = trie_->sibling(src_child_index);
	}

	units_[dest_index].set_num_branches(
		num_units_ - units_[dest_index].leftmost_child());

	src_child_index = trie_->child(src_index);
	while (src_child_index != 0)
	{
		build_trie(src_child_index, dest_child_index);

		src_child_index = trie_->sibling(src_child_index);
		++dest_child_index;
	}
}

inline void TernaryTrieBuilder::build_link()
{
	std::vector<std::pair<UInt32, UInt32> > pairs;
	for (UInt32 index = 0; index < units_.num_objs(); ++index)
	{
		TernaryTrieUnit &unit = units_[index];
		if (unit.label() == '\0')
			continue;

		UInt32 left = unit.leftmost_child();
		UInt32 right = left + unit.num_branches();
		assert(left < right);

		pairs.clear();
		for (UInt32 child_index = left; child_index < right; ++child_index)
		{
			pairs.push_back(std::make_pair(
				units_[child_index].label(), child_index));
		}

		// Each pair consists of a label and a source index.
		std::sort(pairs.begin(), pairs.end());

		for (UInt32 pair_id = 0; pair_id < pairs.size(); ++pair_id)
		{
			pairs[pair_id].first = pairs[pair_id].second;
			pairs[pair_id].second = left + pair_id;
		}

		// Each pair consists of a source index and a desination index.
		std::sort(pairs.begin(), pairs.end());

		if (unit.has_leaf())
		{
			if (pairs.size() > 1)
			{
				unit.set_first_child(pairs[1].second);
				units_[right - 1].set_next_sibling(0);
			}
		}
		else
		{
			unit.set_first_child(pairs[0].second);
			units_[right - 1].set_next_sibling(0);
		}

		for (UInt32 pair_id = 0; pair_id < pairs.size() - 1; ++pair_id)
		{
			units_[pairs[pair_id].first].set_next_sibling(
				pairs[pair_id + 1].second - pairs[pair_id].second);
		}

		std::sort(units_.begin() + left, units_.begin() + right);
	}
}

inline void TernaryTrieBuilder::clear()
{
	trie_ = NULL;
	units_.clear();
	num_units_ = 0;
}

}  // namespace sumire

#endif  // SUMIRE_TERNARY_TRIE_BUILDER_IN_H
