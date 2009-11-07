#ifndef SUMIRE_TRIE_BASE_H
#define SUMIRE_TRIE_BASE_H

#include "int-types.h"

#include <iostream>

namespace sumire {

class TrieBase
{
public:
	static const UInt32 INVALID_VALUE = static_cast<UInt32>(-1);

	TrieBase() {}
	virtual ~TrieBase() {}

	virtual bool build(const TrieBase &trie) = 0;

	bool find(const char *key, UInt32 *value_ptr = NULL) const;
	bool find(const char *key, UInt32 length, UInt32 *value_ptr = NULL) const;

	bool follow(UInt32 index, const char *seq, UInt32 *index_ptr) const;
	bool follow(UInt32 index, const char *seq, UInt32 length,
		UInt32 *index_ptr) const;

	virtual UInt32 find_child(UInt32 index, UInt8 child_label) const = 0;

	virtual UInt32 child(UInt32 index) const = 0;
	virtual UInt32 sibling(UInt32 index) const = 0;
	virtual UInt8 label(UInt32 index) const = 0;

	virtual bool get_value(UInt32 index, UInt32 *value_ptr = NULL) const = 0;
	bool has_value(UInt32 index) const;
	UInt32 value(UInt32 index) const;

	UInt32 root() const;

	virtual UInt32 num_units() const = 0;
	virtual UInt32 num_nodes() const = 0;
	virtual UInt32 num_keys() const = 0;
	virtual UInt32 size() const = 0;

	virtual void clear() = 0;
	virtual void *map(void *addr) = 0;
	virtual bool read(std::istream *input) = 0;
	virtual bool write(std::ostream *output) const = 0;

private:
	// Disallows copies.
	TrieBase(const TrieBase &);
	TrieBase &operator=(const TrieBase &);
};

}  // namespace sumire

#include "trie-base-in.h"

#endif  // SUMIRE_TRIE_BASE_H
