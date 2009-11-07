#ifndef SUMIRE_DA_TRIE_UNIT_H
#define SUMIRE_DA_TRIE_UNIT_H

#include "int-types.h"

namespace sumire {

class DaTrieUnit
{
public:
	DaTrieUnit() : unit_(0), label_('\0'), has_leaf_(false),
		child_label_('\0'), next_sibling_(0) {}

	void set_offset(UInt32 offset) { unit_ = offset; }
	void set_value(UInt32 value) { unit_ = value; }
	void set_label(UInt8 label) { label_ = label; }
	void set_has_leaf() { has_leaf_ = true; }
	void set_child_label(UInt8 label) { child_label_ = label; }
	void set_next_sibling(UInt8 diff) { next_sibling_ = diff; }

	UInt32 offset() const { return unit_; }
	UInt32 value() const { return unit_; }
	UInt32 label() const { return label_; }
	bool has_leaf() const { return has_leaf_; }
	UInt8 child_label() const { return child_label_; }
	UInt8 next_sibling() const { return next_sibling_; }

private:
	UInt32 unit_;
	UInt8 label_;
	bool has_leaf_;
	UInt8 child_label_;
	UInt8 next_sibling_;

	// Copyable.
};

}  // namespace sumire

//#include "da-trie-unit-in.h"

#endif  // SUMIRE_DA_TRIE_UNIT_H
