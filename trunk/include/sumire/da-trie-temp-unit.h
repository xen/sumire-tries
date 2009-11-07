#ifndef SUMIRE_DA_TRIE_TEMP_UNIT_H
#define SUMIRE_DA_TRIE_TEMP_UNIT_H

#include "int-types.h"

namespace sumire {

class DaTrieTempUnit
{
public:
	DaTrieTempUnit() : low_(0), high_(0) {}

	void clear() { low_ = high_ = 0; }

	void set_is_fixed() { low_ |= 1; }
	void set_next(UInt32 next) { low_ = (low_ & 1) | (next << 1); }
	void set_is_used() { high_ |= 1; }
	void set_prev(UInt32 prev) { high_ = (high_ & 1) | (prev << 1); }

	bool is_fixed() const { return (low_ & 1) == 1; }
	UInt32 next() const { return low_ >> 1; }
	bool is_used() const { return (high_ & 1) == 1; }
	UInt32 prev() const { return high_ >> 1; }

private:
	UInt32 low_;
	UInt32 high_;

	// Copyable.
};

}  // namespace sumire

#endif  // SUMIRE_DA_TRIE_TEMP_UNIT_H
