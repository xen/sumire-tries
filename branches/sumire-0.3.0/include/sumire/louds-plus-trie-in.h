#ifndef SUMIRE_LOUDS_PLUS_TRIE_IN_H
#define SUMIRE_LOUDS_PLUS_TRIE_IN_H

#include "object-io.h"

#include <cassert>
#include <queue>

namespace sumire {

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline bool LoudsPlusTrie<SUCCINCT_BIT_VECTOR_TYPE>::build(
	const TrieBase &trie)
{
	if (trie.num_nodes() == 0)
		return false;

	BitVector child_bv, sibling_bv, has_value_bv;

	ObjectArray<UInt8> labels;
	ObjectArray<UInt32> values;
	labels.resize(trie.num_nodes());
	values.resize(trie.num_keys());

	UInt32 label_index = 0;
	UInt32 value_index = 0;

	std::queue<UInt32> queue;
	queue.push(trie.root());
	while (!queue.empty())
	{
		UInt32 index = queue.front();
		queue.pop();

		labels[label_index++] = trie.label(index);

		UInt32 value;
		bool has_value = trie.get_value(index, &value);
		if (has_value)
			values[value_index++] = value;
		has_value_bv.add(has_value);

		UInt32 child_index = trie.child(index);
		child_bv.add(child_index != 0);

		while (child_index != 0)
		{
			queue.push(child_index);
			child_index = trie.sibling(child_index);
		}
		sibling_bv.add(trie.sibling(index) != 0);
	}

	assert(child_bv.num_bits() == trie.num_nodes());
	assert(sibling_bv.num_bits() == trie.num_nodes());
	assert(has_value_bv.num_bits() == trie.num_nodes());
	assert(label_index == labels.num_objs());
	assert(value_index == values.num_objs());

	SuccinctBitVector child_sbv, sibling_sbv, has_value_sbv;
	child_sbv.build(child_bv);
	sibling_sbv.build(sibling_bv);
	has_value_sbv.build(has_value_bv);

	clear();
	child_sbv_.swap(&child_sbv);
	sibling_sbv_.swap(&sibling_sbv);
	has_value_sbv_.swap(&has_value_sbv);
	labels_.swap(&labels);
	values_.swap(&values);

	return true;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline UInt32 LoudsPlusTrie<SUCCINCT_BIT_VECTOR_TYPE>::find_child(
	UInt32 index, UInt8 child_label) const
{
	assert(index < num_units());

	for (UInt32 child_index = child(index); child_index != 0;
		child_index = sibling(child_index))
	{
		if (label(child_index) == child_label)
			return child_index;
	}
	return 0;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline UInt32 LoudsPlusTrie<SUCCINCT_BIT_VECTOR_TYPE>::child(
	UInt32 index) const
{
	assert(index < child_sbv_.num_bits());

	if (!child_sbv_[index])
		return 0;

	UInt32 child_count = child_sbv_.rank_1(index);
	assert(child_count < num_units());

	return child_count + sibling_sbv_.rank_1(
		sibling_sbv_.select_0(child_count));
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline UInt32 LoudsPlusTrie<SUCCINCT_BIT_VECTOR_TYPE>::sibling(
	UInt32 index) const
{
	assert(index < sibling_sbv_.num_bits());

	return sibling_sbv_[index] ? (index + 1) : 0;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline UInt8 LoudsPlusTrie<SUCCINCT_BIT_VECTOR_TYPE>::label(
	UInt32 index) const
{
	assert(index < labels_.num_objs());

	return labels_[index];
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline bool LoudsPlusTrie<SUCCINCT_BIT_VECTOR_TYPE>::get_value(
	UInt32 index, UInt32 *value_ptr) const
{
	assert(index < has_value_sbv_.num_bits());

	if (!has_value_sbv_[index])
		return false;

	if (value_ptr != NULL)
	{
		UInt32 value_id = has_value_sbv_.rank_1(index) - 1;
		assert(value_id < values_.size());

		*value_ptr = values_[value_id];
	}
	return true;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline UInt32 LoudsPlusTrie<SUCCINCT_BIT_VECTOR_TYPE>::size() const
{
	return child_sbv_.size() + sibling_sbv_.size() + has_value_sbv_.size()
		+ labels_.size() + values_.size();
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline void LoudsPlusTrie<SUCCINCT_BIT_VECTOR_TYPE>::clear()
{
	child_sbv_.clear();
	sibling_sbv_.clear();
	has_value_sbv_.clear();
	labels_.clear();
	values_.clear();
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline void *LoudsPlusTrie<SUCCINCT_BIT_VECTOR_TYPE>::map(void *addr)
{
	assert(addr != NULL);

	clear();

	addr = child_sbv_.map(addr);
	addr = sibling_sbv_.map(addr);
	addr = has_value_sbv_.map(addr);
	addr = labels_.map(addr);
	addr = values_.map(addr);

	return addr;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline bool LoudsPlusTrie<SUCCINCT_BIT_VECTOR_TYPE>::read(std::istream *input)
{
	assert(input != NULL);

	SuccinctBitVector child_sbv, sibling_sbv;
	if (!child_sbv.read(input) || !sibling_sbv.read(input))
		return false;

	SuccinctBitVector has_value_sbv;
	if (!has_value_sbv.read(input))
		return false;

	ObjectArray<UInt8> labels;
	if (!labels.read(input))
		return false;

	ObjectArray<UInt32> values;
	if (!values.read(input))
		return false;

	clear();
	child_sbv_.swap(&child_sbv);
	sibling_sbv_.swap(&sibling_sbv);
	has_value_sbv_.swap(&has_value_sbv);
	labels_.swap(&labels);
	values_.swap(&values);

	return true;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline bool LoudsPlusTrie<SUCCINCT_BIT_VECTOR_TYPE>::write(
	std::ostream *output) const
{
	assert(output != NULL);

	if (!child_sbv_.write(output) || !sibling_sbv_.write(output))
		return false;

	if (!has_value_sbv_.write(output))
		return false;

	if (!labels_.write(output) || !values_.write(output))
		return false;

	return true;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline void LoudsPlusTrie<SUCCINCT_BIT_VECTOR_TYPE>::swap(
	LoudsPlusTrie *target)
{
	assert(target != NULL);

	child_sbv_.swap(&target->child_sbv_);
	sibling_sbv_.swap(&target->sibling_sbv_);
	has_value_sbv_.swap(&target->has_value_sbv_);
	labels_.swap(&target->labels_);
	values_.swap(&target->values_);
}

}  // namespace sumire

#endif  // SUMIRE_LOUDS_PLUS_TRIE_IN_H
