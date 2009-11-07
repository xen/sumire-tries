#ifndef SUMIRE_TRIE_BASE_IN_H
#define SUMIRE_TRIE_BASE_IN_H

#include <cassert>

namespace sumire {

inline bool TrieBase::find(const char *key, UInt32 *value_ptr) const
{
	assert(key != NULL);

	UInt32 index;
	if (!follow(root(), key, &index))
		return false;

	return get_value(index, value_ptr);
}

inline bool TrieBase::find(const char *key, UInt32 length,
	UInt32 *value_ptr) const
{
	assert(key != NULL || length == 0);

	UInt32 index;
	if (!follow(root(), key, length, &index))
		return false;

	return get_value(index, value_ptr);
}

inline bool TrieBase::follow(UInt32 index, const char *seq,
	UInt32 *index_ptr = NULL) const
{
	assert(index < num_units());
	assert(seq != NULL);

	while (*seq != '\0')
	{
		index = find_child(index, *seq);
		if (index == 0)
			return false;
		++seq;
	}
	if (index_ptr != NULL)
		*index_ptr = index;
	return true;
}

inline bool TrieBase::follow(UInt32 index, const char *seq,
	UInt32 length, UInt32 *index_ptr = NULL) const
{
	assert(index < num_units());
	assert(seq != NULL || length == 0);

	for (UInt32 i = 0; i < length; ++i)
	{
		index = find_child(index, seq[i]);
		if (index == 0)
			return false;
	}
	if (index_ptr != NULL)
		*index_ptr = index;
	return true;
}

inline bool TrieBase::has_value(UInt32 index) const
{
	assert(index < num_units());

	return get_value(index);
}

inline UInt32 TrieBase::value(UInt32 index) const
{
	assert(index < num_units());

	UInt32 value = INVALID_VALUE;
	get_value(index, &value);
	return value;
}

inline UInt32  TrieBase::root() const { return 0; }

}  // namespace sumire

#endif  // SUMIRE_TRIE_BASE_IN_H
