#ifndef SUMIRE_TERNARY_TRIE_BUILDER_H
#define SUMIRE_TERNARY_TRIE_BUILDER_H

#include "object-array.h"
#include "ternary-trie-unit.h"
#include "trie-base.h"

namespace sumire {

class TernaryTrieBuilder
{
public:
	TernaryTrieBuilder() : trie_(NULL), units_(), num_units_(0) {}
	~TernaryTrieBuilder() { clear(); }

	bool build(const TrieBase &trie, ObjectArray<TernaryTrieUnit> *units);

private:
	const TrieBase *trie_;
	ObjectArray<TernaryTrieUnit> units_;
	UInt32 num_units_;

	// Disallows copies.
	TernaryTrieBuilder(const TernaryTrieBuilder &);
	TernaryTrieBuilder &operator=(const TernaryTrieBuilder &);

	void build_trie(UInt32 src_index, UInt32 dest_index);
	void build_link();

	void clear();
};

}  // namespace sumire

#include "ternary-trie-builder-in.h"

#endif  // SUMIRE_TERNARY_TRIE_BUILDER_H
