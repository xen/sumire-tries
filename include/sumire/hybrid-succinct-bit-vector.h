#ifndef SUMIRE_HYBRID_SUCCINCT_BIT_VECTOR_H
#define SUMIRE_HYBRID_SUCCINCT_BIT_VECTOR_H

#include "bit-vector.h"
#include "object-array.h"

#include <iostream>

namespace sumire {

class HybridSuccinctBitVector
{
public:
	enum { BITS_PER_UNIT = sizeof(UInt32) * 8 };
	enum { BITS_PER_BLOCK = 256 };
	enum { UNITS_PER_BLOCK = BITS_PER_BLOCK / BITS_PER_UNIT };

	static const UInt32 UNIT_0 = 0;
	static const UInt32 UNIT_1 = 1;

	class RankBlock
	{
	public:
		RankBlock();

		void set_rank(UInt32 local_id, UInt32 rank);

		UInt32 rank(UInt32 local_id) const;

		UInt32 global_rank() const { return global_rank_; }
		UInt32 local_rank(UInt32 local_id) const;

	private:
		UInt32 global_rank_;
		UInt8 local_ranks_[UNITS_PER_BLOCK];

		// Copyable.
	};

public:
	HybridSuccinctBitVector() : rank_blocks_(), select_1s_(), select_0s_(),
		units_(), num_bits_(0), num_ones_(0) {}
	~HybridSuccinctBitVector() { clear(); }

	void build(const BitVector &bv);

	UInt32 rank_1(UInt32 index) const;
	UInt32 rank_0(UInt32 index) const;

	UInt32 select_1(UInt32 count) const;
	UInt32 select_0(UInt32 count) const;

	bool operator[](UInt32 index) const;
	bool get(UInt32 index) const;

	UInt32 num_blocks() const { return rank_blocks_.num_objs(); }
	UInt32 num_units() const { return units_.num_objs(); }
	UInt32 num_bits() const { return num_bits_; }
	UInt32 num_ones() const { return num_ones_; }
	UInt32 num_zeros() const { return num_bits_ - num_ones_; }
	UInt32 size() const;

	void clear();
	void *map(void *addr);
	bool read(std::istream *input);
	bool write(std::ostream *output) const;

	void swap(HybridSuccinctBitVector *target);

private:
	ObjectArray<RankBlock> rank_blocks_;
	ObjectArray<UInt32> select_1s_;
	ObjectArray<UInt32> select_0s_;
	ObjectArray<UInt32> units_;
	UInt32 num_bits_;
	UInt32 num_ones_;

	// Disallows copies.
	HybridSuccinctBitVector(const HybridSuccinctBitVector &);
	HybridSuccinctBitVector &operator=(const HybridSuccinctBitVector &);

	void build_select(const ObjectArray<RankBlock> &rank_blocks,
		UInt32 num_ones, UInt32 num_zeros,
		ObjectArray<UInt32> *select_1s, ObjectArray<UInt32> *select_0s);

	static UInt32 pop_count(UInt32 unit);
};

}  // namespace sumire

#include "hybrid-succinct-bit-vector-in.h"

#endif  // SUMIRE_HYBRID_SUCCINCT_BIT_VECTOR_H
