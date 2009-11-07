#ifndef SUMIRE_TRIE_BUILDER_H
#define SUMIRE_TRIE_BUILDER_H

#include "basic-trie.h"
#include "object-pool.h"
#include "trie-builder-node.h"
#include "virtual-trie.h"

#include <vector>

namespace sumire {

class TrieBuilder
{
public:
	TrieBuilder() : node_pool_(), unit_pool_(), label_pool_(),
		unfixed_nodes_(), unused_nodes_(), num_keys_(0), virtual_trie_() {}

	const VirtualTrie &virtual_trie();

	bool insert(const char *key, UInt32 value = 0);
	bool insert(const char *key, UInt32 length, UInt32 value);
	bool finish();

	bool initialized() const;
	bool finished() const;

	UInt32 num_units() const { return unit_pool_.num_objs(); }
	UInt32 num_nodes() const;
	UInt32 num_keys() const { return num_keys_; }
	UInt32 size() const;

	void clear();

	void swap(TrieBuilder *target);

private:
	ObjectPool<TrieBuilderNode> node_pool_;
	ObjectPool<BasicTrieUnit> unit_pool_;
	ObjectPool<UInt8> label_pool_;
	std::vector<UInt32> unfixed_nodes_;
	std::vector<UInt32> unused_nodes_;
	UInt32 num_keys_;
	VirtualTrie virtual_trie_;

	// Disallows copies.
	TrieBuilder(const TrieBuilder &);
	TrieBuilder &operator=(const TrieBuilder &);

	void init();
	void fix_units(UInt32 index);
	UInt32 alloc_node();
	UInt32 alloc_unit();
	void free_node(UInt32 index);
};

}  // namespace sumire

#include "trie-builder-in.h"

#endif  // SUMIRE_TRIE_BUILDER_H
