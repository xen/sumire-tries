#ifndef SUMIRE_SIMPLIFIED_SUCCINCT_BIT_VECTOR_H
#define SUMIRE_SIMPLIFIED_SUCCINCT_BIT_VECTOR_H

#include "bit-vector.h"
#include "object-array.h"

#include <iostream>

namespace sumire {

class SimplifiedSuccinctBitVector
{
public:
	enum { BITS_PER_UNIT = sizeof(UInt32) * 8 };

	static const UInt32 UNIT_0 = 0;
	static const UInt32 UNIT_1 = 1;

	class Unit
	{
	public:
		Unit() : rank_(0), value_(0) {}

		void set_rank(UInt32 rank) { rank_ = rank; }
		void set_value(UInt32 value) { value_ = value; }

		UInt32 rank() const { return rank_; }
		UInt32 value() const { return value_; }

	private:
		UInt32 rank_;
		UInt32 value_;

		// Copyable.
	};

public:
	SimplifiedSuccinctBitVector() : units_(), num_bits_(0), num_ones_(0) {}
	~SimplifiedSuccinctBitVector() { clear(); }

	void build(const BitVector &bv);

	UInt32 rank_1(UInt32 index) const;
	UInt32 rank_0(UInt32 index) const;

	UInt32 select_1(UInt32 count) const;
	UInt32 select_0(UInt32 count) const;

	bool operator[](UInt32 index) const;
	bool get(UInt32 index) const;

	UInt32 num_units() const { return units_.num_objs(); }
	UInt32 num_bits() const { return num_bits_; }
	UInt32 num_ones() const { return num_ones_; }
	UInt32 num_zeros() const { return num_bits_ - num_ones_; }
	UInt32 size() const { return units_.size(); }

	void clear();
	void *map(void *addr);
	bool read(std::istream *input);
	bool write(std::ostream *output) const;

	void swap(SimplifiedSuccinctBitVector *target);

private:
	ObjectArray<Unit> units_;
	UInt32 num_bits_;
	UInt32 num_ones_;

	// Disallows copies.
	SimplifiedSuccinctBitVector(const SimplifiedSuccinctBitVector &);
	SimplifiedSuccinctBitVector &operator=(
		const SimplifiedSuccinctBitVector &);

	static UInt32 pop_count(UInt32 unit);
};

}  // namespace sumire

#include "simplified-succinct-bit-vector-in.h"

#endif  // SUMIRE_SIMPLIFIED_SUCCINCT_BIT_VECTOR_H
