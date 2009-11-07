#ifndef SUMIRE_VALUE_ORDER_COMPLETER_CANDIDATE_H
#define SUMIRE_VALUE_ORDER_COMPLETER_CANDIDATE_H

#include "int-types.h"
#include "trie-base.h"

namespace sumire {

class ValueOrderCompleterCandidate
{
public:
	ValueOrderCompleterCandidate()
		: index_(0), value_(TrieBase::INVALID_VALUE) {}

	void set_index(UInt32 index) { index_ = index; }
	void set_value(UInt32 value) { value_ = value; }

	UInt32 index() const { return index_; }
	UInt32 value() const { return value_; }

	bool operator<(const ValueOrderCompleterCandidate &rhs) const
	{
		if (value_ != rhs.value_)
			return value_ < rhs.value_;
		return index_ > rhs.index_;
	}

private:
	UInt32 index_;
	UInt32 value_;

	// Copyable.
};

}  // namespace sumire

#endif  // SUMIRE_VALUE_ORDER_COMPLETER_CANDIDATE_H
