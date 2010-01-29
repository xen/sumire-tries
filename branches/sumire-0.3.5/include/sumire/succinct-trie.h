#ifndef SUMIRE_SUCCINCT_TRIE_H
#define SUMIRE_SUCCINCT_TRIE_H

#include "basic-succinct-bit-vector.h"
#include "object-array.h"
#include "trie-base.h"

namespace sumire {

template <typename SUCCINCT_BIT_VECTOR_TYPE = BasicSuccinctBitVector>
class SuccinctTrie : public TrieBase
{
public:
	typedef SUCCINCT_BIT_VECTOR_TYPE SuccinctBitVector;

	SuccinctTrie() : tree_sbv_(), has_value_sbv_(), labels_(), values_() {}
	~SuccinctTrie() { clear(); }

	bool build(const TrieBase &trie);

	UInt32 find_child(UInt32 index, UInt8 child_label) const;

	UInt32 child(UInt32 index) const;
	UInt32 sibling(UInt32 index) const;
	UInt8 label(UInt32 index) const;

	bool get_value(UInt32 index, UInt32 *value_ptr = NULL) const;

	UInt32 num_units() const { return tree_sbv_.num_bits(); }
	UInt32 num_nodes() const { return labels_.num_objs(); }
	UInt32 num_keys() const { return values_.num_objs(); }
	UInt32 size() const;

	void clear();
	void *map(void *addr);
	bool read(std::istream *input);
	bool write(std::ostream *output) const;

	void swap(SuccinctTrie *target);

private:
	SuccinctBitVector tree_sbv_;
	SuccinctBitVector has_value_sbv_;
	ObjectArray<UInt8> labels_;
	ObjectArray<UInt32> values_;

	// Disallows copies.
	SuccinctTrie(const SuccinctTrie &);
	SuccinctTrie &operator=(const SuccinctTrie &);
};

}  // namespace sumire

#include "succinct-trie-in.h"

#endif  // SUMIRE_SUCCINCT_TRIE_H
