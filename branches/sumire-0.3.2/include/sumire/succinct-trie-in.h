#ifndef SUMIRE_SUCCINCT_TRIE_IN_H
#define SUMIRE_SUCCINCT_TRIE_IN_H

#include "object-io.h"

#include <cassert>
#include <queue>

namespace sumire {

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline bool SuccinctTrie<SUCCINCT_BIT_VECTOR_TYPE>::build(
	const TrieBase &trie)
{
	if (trie.num_nodes() == 0)
		return false;

	BitVector tree_bv, has_value_bv;

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
		tree_bv.add(child_index != 0);
		if (child_index != 0)
			queue.push(child_index);

		UInt32 sibling_index = trie.sibling(index);
		tree_bv.add(sibling_index != 0);
		if (sibling_index != 0)
			queue.push(sibling_index);
	}

	assert(tree_bv.num_bits() == trie.num_nodes() * 2);
	assert(has_value_bv.num_bits() == trie.num_nodes());
	assert(label_index == labels.num_objs());
	assert(value_index == values.num_objs());

	SuccinctBitVector tree_sbv, has_value_sbv;
	tree_sbv.build(tree_bv);
	has_value_sbv.build(has_value_bv);

	clear();
	tree_sbv_.swap(&tree_sbv);
	has_value_sbv_.swap(&has_value_sbv);
	labels_.swap(&labels);
	values_.swap(&values);

	return true;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline UInt32 SuccinctTrie<SUCCINCT_BIT_VECTOR_TYPE>::find_child(
	UInt32 index, UInt8 child_label) const
{
	for (UInt32 child_index = child(index); child_index != 0;
		child_index = sibling(child_index))
	{
		if (label(child_index) == child_label)
			return child_index;
	}
	return 0;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline UInt32 SuccinctTrie<SUCCINCT_BIT_VECTOR_TYPE>::child(
	UInt32 index) const
{
	assert(index < num_units());

	return tree_sbv_[index] ? (tree_sbv_.rank_1(index) * 2) : 0;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline UInt32 SuccinctTrie<SUCCINCT_BIT_VECTOR_TYPE>::sibling(
	UInt32 index) const
{
	assert(index + 1 < num_units());

	++index;
	return tree_sbv_[index] ? (tree_sbv_.rank_1(index) * 2) : 0;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline UInt8 SuccinctTrie<SUCCINCT_BIT_VECTOR_TYPE>::label(UInt32 index) const
{
	assert(index < num_units());

	UInt32 node_id = index / 2;
	return labels_[node_id];
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline bool SuccinctTrie<SUCCINCT_BIT_VECTOR_TYPE>::get_value(
	UInt32 index, UInt32 *value_ptr) const
{
	assert(index < num_units());

	UInt32 node_id = index / 2;
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
inline UInt32 SuccinctTrie<SUCCINCT_BIT_VECTOR_TYPE>::size() const
{
	return tree_sbv_.size() + has_value_sbv_.size()
		+ labels_.size() + values_.size();
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline void SuccinctTrie<SUCCINCT_BIT_VECTOR_TYPE>::clear()
{
	tree_sbv_.clear();
	has_value_sbv_.clear();
	labels_.clear();
	values_.clear();
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline void *SuccinctTrie<SUCCINCT_BIT_VECTOR_TYPE>::map(void *addr)
{
	assert(addr != NULL);

	clear();

	addr = tree_sbv_.map(addr);
	addr = has_value_sbv_.map(addr);
	addr = labels_.map(addr);
	addr = values_.map(addr);

	return addr;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline bool SuccinctTrie<SUCCINCT_BIT_VECTOR_TYPE>::read(std::istream *input)
{
	assert(input != NULL);

	SuccinctBitVector tree_sbv, has_value_sbv;
	if (!tree_sbv.read(input) || !has_value_sbv.read(input))
		return false;

	ObjectArray<UInt8> labels;
	if (!labels.read(input))
		return false;

	ObjectArray<UInt32> values;
	if (!values.read(input))
		return false;

	clear();
	tree_sbv_.swap(&tree_sbv);
	has_value_sbv_.swap(&has_value_sbv);
	labels_.swap(&labels);
	values_.swap(&values);

	return true;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline bool SuccinctTrie<SUCCINCT_BIT_VECTOR_TYPE>::write(
	std::ostream *output) const
{
	assert(output != NULL);

	if (!tree_sbv_.write(output) || !has_value_sbv_.write(output))
		return false;

	if (!labels_.write(output) || !values_.write(output))
		return false;

	return true;
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
inline void SuccinctTrie<SUCCINCT_BIT_VECTOR_TYPE>::swap(SuccinctTrie *target)
{
	assert(target != NULL);

	tree_sbv_.swap(&target->tree_sbv_);
	has_value_sbv_.swap(&target->has_value_sbv_);
	labels_.swap(&target->labels_);
	values_.swap(&target->values_);
}

}  // namespace sumire

#endif  // SUMIRE_SUCCINCT_TRIE_IN_H
