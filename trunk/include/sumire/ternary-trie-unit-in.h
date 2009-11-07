#ifndef SUMIRE_TERNARY_TRIE_UNIT_IN_H
#define SUMIRE_TERNARY_TRIE_UNIT_IN_H

#include <cassert>

namespace sumire {

inline void TernaryTrieUnit::set_leftmost_child(UInt32 index)
{
	assert(index <= (UInt32Limits::max() >> 1));

	base_ = (base_ & 1) | (index << 1);
}

inline void TernaryTrieUnit::set_value(UInt32 value)
{
	assert(value <= (UInt32Limits::max() >> 1));

	base_ = (base_ & 1) | (value << 1);
}

inline void TernaryTrieUnit::set_has_leaf()
{
	assert(has_leaf() == false);

	base_ |= 1;
}

inline void TernaryTrieUnit::set_label(UInt8 label)
{
	label_ = label;
}

inline void TernaryTrieUnit::set_num_branches(UInt32 num_branches)
{
	assert(num_branches > 0);
	assert(num_branches <= 256);

	num_branches_ = num_branches - 1;
}

inline void TernaryTrieUnit::set_first_child(UInt32 index)
{
	assert(index <= (UInt32Limits::max() >> 1));

	first_child_ = index - leftmost_child();
}

inline void TernaryTrieUnit::set_next_sibling(int rel_index)
{
	assert(rel_index + label_ > 0);
	assert(rel_index + label_ < 256);

	next_sibling_ = rel_index + label_;
}

inline UInt32 TernaryTrieUnit::leftmost_child() const
{
	assert(label_ != '\0');

	return base_ >> 1;
}

inline UInt32 TernaryTrieUnit::value() const
{
	assert(label_ == '\0');

	return base_ >> 1;
}

inline bool TernaryTrieUnit::has_leaf() const
{
	return (base_ & 1) ? true : false;
}

inline UInt8 TernaryTrieUnit::label() const
{
	return label_;
}

inline UInt32 TernaryTrieUnit::num_branches() const
{
	assert(label_ != '\0');

	return num_branches_ + 1;
}

inline UInt32 TernaryTrieUnit::first_child() const
{
	assert(label_ != '\0');

	return leftmost_child() + first_child_;
}

inline int TernaryTrieUnit::next_sibling() const
{
	return next_sibling_ - label_;
}

inline bool TernaryTrieUnit::operator<(const TernaryTrieUnit &rhs) const
{
	return label_ < rhs.label_;
}

}  // namespace sumire

#endif  // SUMIRE_TERNARY_TRIE_UNIT_IN_H
