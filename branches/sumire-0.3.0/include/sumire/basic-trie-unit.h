#ifndef SUMIRE_BASIC_TRIE_UNIT_H
#define SUMIRE_BASIC_TRIE_UNIT_H

#include "int-types.h"

namespace sumire {

class BasicTrieUnit
{
public:
	BasicTrieUnit() : unit_(0) {}

	void set_unit(UInt32 unit) { unit_ = unit; }
	UInt32 unit() const { return unit_; }

	void set_child(UInt32 child) { unit_ = (unit_ & 1) | (child << 1); }
	void set_value(UInt32 value) { set_child(value); }
	void set_has_sibling() { unit_ |= 1; }

	UInt32 child() const { return unit_ >> 1; }
	UInt32 value() const { return child(); }
	bool has_sibling() const { return (unit_ & 1) ? true : false; }

private:
	UInt32 unit_;

	// Copyable.
};

}  // namespace sumire

#endif  // SUMIRE_BASIC_TRIE_UNIT_H
