#ifndef SUMIRE_TERNARY_TRIE_UNIT_H
#define SUMIRE_TERNARY_TRIE_UNIT_H

#include "int-types.h"

namespace sumire {

class TernaryTrieUnit
{
public:
	TernaryTrieUnit() : base_(0), label_('\0'), num_branches_(0),
		first_child_(0), next_sibling_(0) {}

	void set_leftmost_child(UInt32 index);
	void set_value(UInt32 value);
	void set_has_leaf();

	void set_label(UInt8 label);
	void set_num_branches(UInt32 num_branches);

	void set_first_child(UInt32 index);
	void set_next_sibling(int rel_index);

	UInt32 leftmost_child() const;
	UInt32 value() const;
	bool has_leaf() const;

	UInt8 label() const;
	UInt32 num_branches() const;

	UInt32 first_child() const;
	int next_sibling() const;

	bool operator<(const TernaryTrieUnit &rhs) const;

private:
	UInt32 base_;
	UInt8 label_;
	UInt8 num_branches_;
	UInt8 first_child_;
	UInt8 next_sibling_;

	// Copyable.
};

}  // namespace sumire

#include "ternary-trie-unit-in.h"

#endif  // SUMIRE_TERNARY_TRIE_UNIT_H
