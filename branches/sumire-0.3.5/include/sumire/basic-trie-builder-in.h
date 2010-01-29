#ifndef SUMIRE_BASIC_TRIE_BUILDER_IN_H
#define SUMIRE_BASIC_TRIE_BUILDER_IN_H

#include <algorithm>
#include <cassert>
#include <numeric>
#include <queue>

namespace sumire {

inline bool BasicTrieBuilder::build(const TrieBase &trie,
	ObjectArray<BasicTrieUnit> *units, ObjectArray<UInt8> *labels)
{
	assert(units != NULL);
	assert(labels != NULL);

	if (!init(trie))
		return false;

	if (trie_->num_nodes() > 1)
		build(trie_->root(), 0);

	finish(units, labels);

	return true;
}

inline bool BasicTrieBuilder::build_in_level_order(const TrieBase &trie,
	ObjectArray<BasicTrieUnit> *units, ObjectArray<UInt8> *labels)
{
	assert(units != NULL);
	assert(labels != NULL);

	if (!init(trie))
		return false;

	if (trie_->num_nodes() > 1)
		build_in_level_order();

	finish(units, labels);

	return true;
}

inline bool BasicTrieBuilder::build_in_breadth_order(const TrieBase &trie,
	ObjectArray<BasicTrieUnit> *units, ObjectArray<UInt8> *labels)
{
	assert(units != NULL);
	assert(labels != NULL);

	if (!init(trie))
		return false;

	if (trie_->num_nodes() > 1)
	{
		std::vector<BasicTrieBuilderTuple> tuples;
		build_in_value_order(trie_->root(), 0, &tuples,
			get_one, get_total_value);
		assert(tuples.size() == 1);
	}

	finish(units, labels);

	return true;
}

inline bool BasicTrieBuilder::build_in_max_value_order(const TrieBase &trie,
	ObjectArray<BasicTrieUnit> *units, ObjectArray<UInt8> *labels)
{
	assert(units != NULL);
	assert(labels != NULL);

	if (!init(trie))
		return false;

	if (trie_->num_nodes() > 1)
	{
		std::vector<BasicTrieBuilderTuple> tuples;
		build_in_value_order(trie_->root(), 0, &tuples,
			get_value, get_last_value);
		assert(tuples.size() == 1);
	}

	finish(units, labels);

	return true;
}

inline bool BasicTrieBuilder::build_in_total_value_order(const TrieBase &trie,
	ObjectArray<BasicTrieUnit> *units, ObjectArray<UInt8> *labels)
{
	assert(units != NULL);
	assert(labels != NULL);

	if (!init(trie))
		return false;

	if (trie_->num_nodes() > 1)
	{
		std::vector<BasicTrieBuilderTuple> tuples;
		build_in_value_order(trie_->root(), 0, &tuples,
			get_value, get_total_value);
		assert(tuples.size() == 1);
	}

	finish(units, labels);

	return true;
}

inline void BasicTrieBuilder::build(UInt32 src_index, UInt32 dest_index)
{
	assert(src_index < trie_->num_units());
	assert(dest_index < num_units_);

	units_[dest_index].set_child(num_units_);

	UInt32 value;
	bool has_value = trie_->get_value(src_index, &value);
	bool has_sibling = false;
	if (has_value)
	{
		UInt32 leaf_index = num_units_++;
		labels_[leaf_index] = '\0';
		units_[leaf_index].set_value(value);
		has_sibling = true;
	}

	UInt32 dest_child_index = num_units_;

	UInt32 src_child_index = trie_->child(src_index);
	while (src_child_index != 0)
	{
		if (has_sibling)
			units_[num_units_ - 1].set_has_sibling();
		has_sibling = true;

		labels_[num_units_++] = trie_->label(src_child_index);
		src_child_index = trie_->sibling(src_child_index);
	}

	src_child_index = trie_->child(src_index);
	while (src_child_index != 0)
	{
		build(src_child_index, dest_child_index);

		src_child_index = trie_->sibling(src_child_index);
		++dest_child_index;
	}
}

inline void BasicTrieBuilder::build_in_level_order()
{
	std::queue<std::pair<UInt32, UInt32> > queue;
	queue.push(std::make_pair(trie_->root(), 0));

	while (!queue.empty())
	{
		std::pair<UInt32, UInt32> pair = queue.front();
		queue.pop();

		units_[pair.second].set_child(num_units_);

		UInt32 value;
		bool has_value = trie_->get_value(pair.first, &value);
		bool has_sibling = false;
		if (has_value)
		{
			UInt32 leaf_index = num_units_++;
			labels_[leaf_index] = '\0';
			units_[leaf_index].set_value(value);
			has_sibling = true;
		}

		UInt32 src_child_index = trie_->child(pair.first);
		while (src_child_index != 0)
		{
			if (has_sibling)
				units_[num_units_ - 1].set_has_sibling();
			has_sibling = true;

			UInt32 dest_child_index = num_units_++;
			queue.push(std::make_pair(src_child_index, dest_child_index));
			labels_[dest_child_index] = trie_->label(src_child_index);
			src_child_index = trie_->sibling(src_child_index);
		}
	}
}

template <typename GET_LEAF_VALUE_FUNC,
	typename GET_NON_LEAF_VALUE_FUNC>
inline void BasicTrieBuilder::build_in_value_order(UInt32 src_index,
	UInt32 dest_index, std::vector<BasicTrieBuilderTuple> *tuples,
	GET_LEAF_VALUE_FUNC get_leaf_value,
	GET_NON_LEAF_VALUE_FUNC get_non_leaf_value)
{
	assert(src_index < trie_->num_units());
	assert(dest_index < num_units_);
	assert(tuples != NULL);

	units_[dest_index].set_child(num_units_);

	UInt32 tuples_begin = tuples->size();

	UInt32 value;
	bool has_value = trie_->get_value(src_index, &value);
	bool has_sibling = false;
	if (has_value)
	{
		tuples->push_back(BasicTrieBuilderTuple(
			value, '\0', get_leaf_value(value)));
		has_sibling = true;
		++num_units_;
	}

	UInt32 dest_child_index = num_units_;

	UInt32 src_child_index = trie_->child(src_index);
	while (src_child_index != 0)
	{
		if (has_sibling)
			units_[num_units_ - 1].set_has_sibling();
		has_sibling = true;
		++num_units_;

		src_child_index = trie_->sibling(src_child_index);
	}

	src_child_index = trie_->child(src_index);
	while (src_child_index != 0)
	{
		build_in_value_order(src_child_index, dest_child_index, tuples,
			get_leaf_value, get_non_leaf_value);

		src_child_index = trie_->sibling(src_child_index);
		++dest_child_index;
	}

	assert(tuples_begin < tuples->size());

	if (has_value)
	{
		std::swap(*(tuples->begin() + tuples_begin), *(tuples->end() - 1));
		std::stable_sort(tuples->begin() + tuples_begin, tuples->end() - 1);
	}
	else
		std::stable_sort(tuples->begin() + tuples_begin, tuples->end());

	for (UInt32 tuple_id = tuples_begin; tuple_id < tuples->size(); ++tuple_id)
	{
		--dest_child_index;
		units_[dest_child_index].set_child((*tuples)[tuple_id].child());
		labels_[dest_child_index] = (*tuples)[tuple_id].label();
	}

	assert(tuples->size() > tuples_begin);

	value = get_non_leaf_value(tuples->begin() + tuples_begin, tuples->end());
	tuples->resize(tuples_begin);
	tuples->push_back(BasicTrieBuilderTuple(
		dest_child_index, trie_->label(src_index), value));
}

inline bool BasicTrieBuilder::init(const TrieBase &trie)
{
	if (trie.num_nodes() == 0)
		return false;

	clear();

	trie_ = &trie;
	units_.resize(trie_->num_nodes() + trie_->num_keys());
	labels_.resize(trie_->num_nodes() + trie_->num_keys());

	num_units_++;
	labels_[0] = trie_->label(trie_->root());

	return true;
}

inline void BasicTrieBuilder::finish(ObjectArray<BasicTrieUnit> *units,
	ObjectArray<UInt8> *labels)
{
	assert(units != NULL);
	assert(labels != NULL);
	assert(num_units_ == units_.num_objs());

	units_.swap(units);
	labels_.swap(labels);

	clear();
}

inline void BasicTrieBuilder::clear()
{
	trie_ = NULL;
	units_.clear();
	labels_.clear();
	num_units_ = 0;
}

inline UInt64 BasicTrieBuilder::get_last_value(
	std::vector<BasicTrieBuilderTuple>::const_iterator begin,
	std::vector<BasicTrieBuilderTuple>::const_iterator end)
{
	assert(begin < end);

	--end;
	return (begin == end) ? end->value() :
		std::max(end->value(), (end - 1)->value());
}

inline UInt64 BasicTrieBuilder::get_total_value(
	std::vector<BasicTrieBuilderTuple>::const_iterator begin,
	std::vector<BasicTrieBuilderTuple>::const_iterator end)
{
	UInt64 total_value = 0;
	for (std::vector<BasicTrieBuilderTuple>::const_iterator it = begin;
		it != end; ++it)
		total_value += it->value();
	return total_value;
}

}  // namespace sumire

#endif  // SUMIRE_BASIC_TRIE_BUILDER_IN_H
