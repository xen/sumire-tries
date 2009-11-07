#ifndef SUMIRE_TRIE_BUILDER_NODE_H
#define SUMIRE_TRIE_BUILDER_NODE_H

#include "int-types.h"

namespace sumire {

class TrieBuilderNode
{
public:
	TrieBuilderNode() : child_(0), sibling_(0), label_('\0'),
		has_sibling_(false) {}

	void set_child(UInt32 child) { child_ = child; }
	void set_sibling(UInt32 sibling) { sibling_ = sibling; }
	void set_value(UInt32 value) { set_child(value); }
	void set_label(UInt8 label) { label_ = label; }
	void set_has_sibling(bool has_sibling) { has_sibling_ = has_sibling; }

	UInt32 child() const { return child_; }
	UInt32 sibling() const { return sibling_; }
	UInt32 value() const { return child(); }
	UInt8 label() const { return label_; }
	bool has_sibling() const { return has_sibling_; }
	bool has_child() const { return child_ != 0; }

	UInt32 unit() const { return (child_ << 1) | (has_sibling_ ? 1 : 0); }

	void clear()
	{
		child_ = 0;
		sibling_ = 0;
		label_ = '\0';
		has_sibling_ = false;
	}

private:
	UInt32 child_;
	UInt32 sibling_;
	UInt8 label_;
	bool has_sibling_;

	// Copyable.
};

}  // namespace sumire

#endif  // SUMIRE_TRIE_BUILDER_NODE_H
