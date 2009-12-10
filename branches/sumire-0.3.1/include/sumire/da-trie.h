#ifndef SUMIRE_DA_TRIE_H
#define SUMIRE_DA_TRIE_H

#include "basic-trie.h"
#include "da-trie-unit.h"
#include "object-array.h"
#include "trie-base.h"

namespace sumire {

class DaTrie : public TrieBase
{
public:
	DaTrie() : units_() {}
	~DaTrie() { clear(); }

	bool build(const TrieBase &trie);

	UInt32 find_child(UInt32 index, UInt8 child_label) const;

	UInt32 child(UInt32 index) const;
	UInt32 sibling(UInt32 index) const;
	UInt8 label(UInt32 index) const;

	bool get_value(UInt32 index, UInt32 *value_ptr = NULL) const;

	UInt32 num_units() const { return units_.num_objs(); }
	UInt32 num_nodes() const { return num_nodes_; }
	UInt32 num_keys() const { return num_keys_; }
	UInt32 size() const { return units_.size(); }

	void clear();
	void *map(void *addr);
	bool read(std::istream *input);
	bool write(std::ostream *output) const;

	void swap(DaTrie *target);

private:
	ObjectArray<DaTrieUnit> units_;
	UInt32 num_nodes_;
	UInt32 num_keys_;

	// Disallows copies.
	DaTrie(const DaTrie &);
	DaTrie &operator=(const DaTrie &);
};

}  // namespace sumire

#include "da-trie-in.h"

#endif  // SUMIRE_DA_TRIE_H
