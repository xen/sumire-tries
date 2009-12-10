#ifndef SUMIRE_TERNARY_TRIE_H
#define SUMIRE_TERNARY_TRIE_H

#include "basic-trie.h"
#include "ternary-trie-unit.h"
#include "trie-base.h"

namespace sumire {

class TernaryTrie : public TrieBase
{

public:
	TernaryTrie() : units_(), num_keys_(0) {}
	~TernaryTrie() { clear(); }

	bool build(const TrieBase &trie);

	UInt32 find_child(UInt32 index, UInt8 child_label) const;

	UInt32 child(UInt32 index) const;
	UInt32 sibling(UInt32 index) const;
	UInt8 label(UInt32 index) const;

	bool get_value(UInt32 index, UInt32 *value_ptr = NULL) const;

	UInt32 num_units() const { return units_.num_objs(); }
	UInt32 num_nodes() const;
	UInt32 num_keys() const { return num_keys_; }
	UInt32 size() const { return units_.size(); }

	void clear();
	void *map(void *addr);
	bool read(std::istream *input);
	bool write(std::ostream *output) const;

	void swap(TernaryTrie *target);

private:
	ObjectArray<TernaryTrieUnit> units_;
	UInt32 num_keys_;

	// Disallows copies.
	TernaryTrie(const TernaryTrie &);
	TernaryTrie &operator=(const TernaryTrie &);
};

}  // namespace sumire

#include "ternary-trie-in.h"

#endif  // SUMIRE_TERNARY_TRIE_H
