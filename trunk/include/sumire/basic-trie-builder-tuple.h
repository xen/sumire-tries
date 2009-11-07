#ifndef SUMIRE_BASIC_TRIE_BUILDER_TUPLE_H
#define SUMIRE_BASIC_TRIE_BUILDER_TUPLE_H

#include "int-types.h"

namespace sumire {

class BasicTrieBuilderTuple
{
public:
	BasicTrieBuilderTuple() : child_(0), label_('\0'), value_(0) {}
	BasicTrieBuilderTuple(UInt32 child, UInt8 label, UInt64 value)
		: child_(child), label_(label), value_(value) {}

	UInt32 child() const { return child_; }
	UInt8 label() const { return label_; }
	UInt64 value() const { return value_; }

	bool operator<(const BasicTrieBuilderTuple &rhs) const
	{
		return value_ < rhs.value_;
	}

private:
	UInt32 child_;
	UInt8 label_;
	UInt64 value_;

	// Copyable.
};

}  // namespace sumire

#endif  // SUMIRE_BASIC_TRIE_BUILDER_TUPLE_H
