#ifndef SUMIRE_LOUDS_TRIE_IN_H
#define SUMIRE_LOUDS_TRIE_IN_H

#include "object-io.h"

#include <cassert>
#include <queue>

namespace sumire {

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline bool LoudsTrie<SUCCINCT_BIT_VECTOR_TYPE>::build(const TrieBase &trie)
{
	if (trie.num_nodes() == 0)
		return false;

	BitVector louds_bv, has_value_bv;
	louds_bv.add(true);
	louds_bv.add(false);

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

		for (UInt32 child_index = trie.child(index); child_index != 0;
			child_index = trie.sibling(child_index))
		{
			louds_bv.add(true);
			queue.push(child_index);
		}
		louds_bv.add(false);
	}

	assert(louds_bv.num_bits() == (trie.num_nodes() * 2) + 1);
	assert(has_value_bv.num_bits() == trie.num_nodes());
	assert(label_index == labels.num_objs());
	assert(value_index == values.num_objs());

	SuccinctBitVector louds_sbv, has_value_sbv;
	louds_sbv.build(louds_bv);
	has_value_sbv.build(has_value_bv);

	clear();
	louds_sbv_.swap(&louds_sbv);
	has_value_sbv_.swap(&has_value_sbv);
	labels_.swap(&labels);
	values_.swap(&values);

	return true;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline UInt32 LoudsTrie<SUCCINCT_BIT_VECTOR_TYPE>::find_child(
	UInt32 index, UInt8 child_label) const
{
	UInt32 child_index = child(index);
	if (child_index != 0)
	{
		UInt32 node_index = louds_sbv_.rank_1(child_index) - 1;
		do
		{
			if (labels_[node_index] == child_label)
				return child_index;
			child_index = sibling(child_index);
			++node_index;
		} while (child_index != 0);
	}
	return 0;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline UInt32 LoudsTrie<SUCCINCT_BIT_VECTOR_TYPE>::child(UInt32 index) const
{
	assert(index < num_units());

	UInt32 child_index = louds_sbv_.select_0(louds_sbv_.rank_1(index)) + 1;
	return louds_sbv_[child_index] ? child_index : 0;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline UInt32 LoudsTrie<SUCCINCT_BIT_VECTOR_TYPE>::sibling(UInt32 index) const
{
	assert(index < num_units());

	return louds_sbv_[index + 1] ? (index + 1) : 0;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline UInt8 LoudsTrie<SUCCINCT_BIT_VECTOR_TYPE>::label(UInt32 index) const
{
	assert(index < num_units());

	return labels_[louds_sbv_.rank_1(index) - 1];
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline bool LoudsTrie<SUCCINCT_BIT_VECTOR_TYPE>::get_value(
	UInt32 index, UInt32 *value_ptr) const
{
	assert(index < num_units());

	UInt32 node_id = louds_sbv_.rank_1(index) - 1;
	assert(node_id < num_nodes());

	if (!has_value_sbv_[node_id])
		return false;

	if (value_ptr != NULL)
	{
		UInt32 value_id = has_value_sbv_.rank_1(node_id) - 1;
		assert(value_id < num_keys());

		*value_ptr = values_[value_id];
	}
	return true;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline UInt32 LoudsTrie<SUCCINCT_BIT_VECTOR_TYPE>::size() const
{
	return louds_sbv_.size() + has_value_sbv_.size()
		+ labels_.size() + values_.size();
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline void LoudsTrie<SUCCINCT_BIT_VECTOR_TYPE>::clear()
{
	louds_sbv_.clear();
	has_value_sbv_.clear();
	labels_.clear();
	values_.clear();
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline void *LoudsTrie<SUCCINCT_BIT_VECTOR_TYPE>::map(void *addr)
{
	assert(addr != NULL);

	clear();

	addr = louds_sbv_.map(addr);
	addr = has_value_sbv_.map(addr);
	addr = labels_.map(addr);
	addr = values_.map(addr);

	return addr;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline bool LoudsTrie<SUCCINCT_BIT_VECTOR_TYPE>::read(std::istream *input)
{
	assert(input != NULL);

	SuccinctBitVector louds_sbv, has_value_sbv;
	if (!louds_sbv.read(input) || !has_value_sbv.read(input))
		return false;

	ObjectArray<UInt8> labels;
	if (!labels.read(input))
		return false;

	ObjectArray<UInt32> values;
	if (!values.read(input))
		return false;

	clear();
	louds_sbv_.swap(&louds_sbv);
	has_value_sbv_.swap(&has_value_sbv);
	labels_.swap(&labels);
	values_.swap(&values);

	return true;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline bool LoudsTrie<SUCCINCT_BIT_VECTOR_TYPE>::write(
	std::ostream *output) const
{
	assert(output != NULL);

	if (!louds_sbv_.write(output) || !has_value_sbv_.write(output))
		return false;

	if (!labels_.write(output) || !values_.write(output))
		return false;

	return true;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline void LoudsTrie<SUCCINCT_BIT_VECTOR_TYPE>::swap(LoudsTrie *target)
{
	assert(target != NULL);

	louds_sbv_.swap(&target->louds_sbv_);
	has_value_sbv_.swap(&target->has_value_sbv_);
	labels_.swap(&target->labels_);
	values_.swap(&target->values_);
}

}  // namespace sumire

#endif  // SUMIRE_LOUDS_TRIE_IN_H
