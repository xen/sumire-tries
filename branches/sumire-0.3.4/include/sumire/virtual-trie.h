#ifndef SUMIRE_VIRTUAL_TRIE_H
#define SUMIRE_VIRTUAL_TRIE_H

#include "basic-trie-unit.h"
#include "object-pool.h"
#include "trie-base.h"

namespace sumire {

class VirtualTrie : public TrieBase
{
public:
	VirtualTrie() : unit_pool_(NULL), label_pool_(NULL), num_keys_(0) {}
	VirtualTrie(const ObjectPool<BasicTrieUnit> &unit_pool,
		const ObjectPool<UInt8> &label_pool, UInt32 num_keys);
	~VirtualTrie() { clear(); }

	bool build(const TrieBase &trie) { return false; }

	UInt32 find_child(UInt32 index, UInt8 child_label) const;

	UInt32 child(UInt32 index) const;
	UInt32 sibling(UInt32 index) const;
	UInt8 label(UInt32 index) const;

	bool get_value(UInt32 index, UInt32 *value_ptr = NULL) const;

	UInt32 num_units() const { return unit_pool_->num_objs(); }
	UInt32 num_nodes() const;
	UInt32 num_keys() const { return num_keys_; }
	UInt32 size() const { return unit_pool_->size() + label_pool_->size(); }

	void clear();
	void *map(void *addr) { return addr; }
	bool read(std::istream *input) { return false; }
	bool write(std::ostream *output) const;

	void swap(VirtualTrie *target);

private:
	const ObjectPool<BasicTrieUnit> *unit_pool_;
	const ObjectPool<UInt8> *label_pool_;
	UInt32 num_keys_;

	// Disallows copies.
	VirtualTrie(const VirtualTrie &);
	VirtualTrie &operator=(const VirtualTrie &);
};

}  // namespace sumire

#include "virtual-trie-in.h"

#endif  // SUMIRE_VIRTUAL_TRIE_H
