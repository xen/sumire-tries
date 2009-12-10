#ifndef SUMIRE_BASIC_TRIE_H
#define SUMIRE_BASIC_TRIE_H

#include "basic-trie-unit.h"
#include "object-array.h"
#include "trie-base.h"

#include <iostream>

namespace sumire {

class BasicTrie : public TrieBase
{
public:
	enum Order
	{
		LEVEL_ORDER,
		BREADTH_ORDER,
		MAX_VALUE_ORDER,
		TOTAL_VALUE_ORDER
	};

	bool build(const TrieBase &trie, Order order);

public:
	BasicTrie() : units_(), labels_(), num_keys_(0) {}
	~BasicTrie() { clear(); }

	bool build(const TrieBase &trie);

	UInt32 find_child(UInt32 index, UInt8 child_label) const;

	UInt32 child(UInt32 index) const;
	UInt32 sibling(UInt32 index) const;
	UInt8 label(UInt32 index) const;

	bool get_value(UInt32 index, UInt32 *value_ptr = NULL) const;

	UInt32 num_units() const { return units_.num_objs(); }
	UInt32 num_nodes() const;
	UInt32 num_keys() const { return num_keys_; }
	UInt32 size() const { return units_.size() + labels_.size(); }

	void clear();
	void *map(void *addr);
	bool read(std::istream *input);
	bool write(std::ostream *output) const;

	void swap(BasicTrie *target);

private:
	ObjectArray<BasicTrieUnit> units_;
	ObjectArray<UInt8> labels_;
	UInt32 num_keys_;

	// Disallows copies.
	BasicTrie(const BasicTrie &);
	BasicTrie &operator=(const BasicTrie &);
};

}  // namespace sumire

#include "basic-trie-in.h"

#endif  // SUMIRE_BASIC_TRIE_H
