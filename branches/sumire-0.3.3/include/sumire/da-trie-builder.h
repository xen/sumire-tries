#ifndef SUMIRE_DA_TRIE_BUILDER_H
#define SUMIRE_DA_TRIE_BUILDER_H

#include "da-trie-temp-unit.h"
#include "da-trie-unit.h"
#include "object-pool.h"
#include "trie-base.h"

#include <vector>

namespace sumire {

class DaTrieBuilder
{
public:
	DaTrieBuilder() : trie_(NULL), unit_pool_(), temp_blocks_(),
		labels_(), unfixed_index_(0) {}
	~DaTrieBuilder() { clear(); }

	bool build(const TrieBase &trie, ObjectArray<DaTrieUnit> *units_ptr);

private:
	const TrieBase *trie_;
	ObjectPool<DaTrieUnit> unit_pool_;
	std::vector<DaTrieTempUnit *> temp_blocks_;
	std::vector<UInt8> labels_;
	UInt32 unfixed_index_;

	// Disallows copies.
	DaTrieBuilder(const DaTrieBuilder &);
	DaTrieBuilder &operator=(const DaTrieBuilder &);

	enum { UNITS_PER_BLOCK = 256 };
	enum { NUM_UNFIXED_BLOCKS = 16 };

	void build_da(UInt32 trie_index, UInt32 da_index);

	UInt32 arrange_nodes(UInt32 trie_index, UInt32 da_index);
	UInt32 find_valid_offset(UInt32 index) const;
	bool is_valid_offset(UInt32 index, UInt32 offset) const;

	void fix_all();
	void fix_block(UInt32 block_id);

	void reserve(UInt32 index);
	void expand();

	const DaTrieUnit &unit(UInt32 index) const;
	const DaTrieTempUnit &temp(UInt32 index) const;
	DaTrieUnit &unit(UInt32 index);
	DaTrieTempUnit &temp(UInt32 index);

	UInt32 num_units() const { return unit_pool_.num_objs(); }
	UInt32 num_blocks() const { return temp_blocks_.size(); }

	void clear();
};

}  // namespace sumire

#include "da-trie-builder-in.h"

#endif  // SUMIRE_DA_TRIE_BUILDER_H
