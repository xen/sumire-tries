#ifndef SUMIRE_BASIC_TRIE_BUILDER_H
#define SUMIRE_BASIC_TRIE_BUILDER_H

#include "basic-trie-builder-tuple.h"
#include "basic-trie-unit.h"
#include "object-array.h"

#include <vector>

namespace sumire {

class BasicTrieBuilder
{
public:
	BasicTrieBuilder() : trie_(NULL), units_(), labels_(), num_units_(0) {}

	bool build(const TrieBase &trie,
		ObjectArray<BasicTrieUnit> *units, ObjectArray<UInt8> *labels);
	bool build_in_level_order(const TrieBase &trie,
		ObjectArray<BasicTrieUnit> *units, ObjectArray<UInt8> *labels);
	bool build_in_breadth_order(const TrieBase &trie,
		ObjectArray<BasicTrieUnit> *units, ObjectArray<UInt8> *labels);
	bool build_in_max_value_order(const TrieBase &trie,
		ObjectArray<BasicTrieUnit> *units, ObjectArray<UInt8> *labels);
	bool build_in_total_value_order(const TrieBase &trie,
		ObjectArray<BasicTrieUnit> *units, ObjectArray<UInt8> *labels);

private:
	const TrieBase *trie_;
	ObjectArray<BasicTrieUnit> units_;
	ObjectArray<UInt8> labels_;
	UInt32 num_units_;

	// Disallows copies.
	BasicTrieBuilder(const BasicTrieBuilder &);
	BasicTrieBuilder &operator=(const BasicTrieBuilder &);

	void build(UInt32 src_index, UInt32 dest_index);
	void build_in_level_order();

	template <typename GET_LEAF_VALUE_FUNC,
		typename GET_NON_LEAF_VALUE_FUNC>
	void build_in_value_order(UInt32 src_index, UInt32 dest_index,
		std::vector<BasicTrieBuilderTuple> *tuples,
		GET_LEAF_VALUE_FUNC get_leaf_value,
		GET_NON_LEAF_VALUE_FUNC get_non_leaf_value);

	bool init(const TrieBase &trie);
	void finish(ObjectArray<BasicTrieUnit> *units, ObjectArray<UInt8> *labels);

	void clear();

private:
	static UInt64 get_one(UInt32) { return 1; }
	static UInt64 get_value(UInt32 value) { return value; }

	static UInt64 get_last_value(
		std::vector<BasicTrieBuilderTuple>::const_iterator,
		std::vector<BasicTrieBuilderTuple>::const_iterator end);
	static UInt64 get_total_value(
		std::vector<BasicTrieBuilderTuple>::const_iterator begin,
		std::vector<BasicTrieBuilderTuple>::const_iterator end);
};

}  // namespace sumire

#include "basic-trie-builder-in.h"

#endif  // SUMIRE_BASIC_TRIE_BUILDER_H
