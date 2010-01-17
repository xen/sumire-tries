#ifndef SUMIRE_BIT_VECTOR_H
#define SUMIRE_BIT_VECTOR_H

#include "object-pool.h"

#include <iostream>

namespace sumire {

class BitVector
{
public:
	enum { BITS_PER_UNIT = sizeof(UInt32) * 8 };

	static const UInt32 UNIT_0 = 0;
	static const UInt32 UNIT_1 = 1;

	class Accessor
	{
	public:
		Accessor(BitVector *bv, UInt32 index) : bv_(bv), index_(index) {}

		operator bool() const { return bv_->get(index_); }
		Accessor &operator=(bool bit);

	private:
		BitVector *bv_;
		UInt32 index_;

		// Copyable.
	};

public:
	BitVector() : unit_pool_(), num_bits_(0) {}
	~BitVector() { clear(); }

	bool operator[](UInt32 index) const { return get(index); }
	Accessor operator[](UInt32 index) { return Accessor(this, index); }

	bool get(UInt32 index) const;
	void set(UInt32 index, bool bit);
	void add(bool bit);

	UInt32 unit(UInt32 unit_id) const;

	UInt32 num_units() const { return unit_pool_.num_objs(); }
	UInt32 num_bits() const { return num_bits_; }
	UInt32 size() const { return unit_pool_.size(); }
	UInt32 capacity() const { return num_units() * BITS_PER_UNIT; }

	void resize(UInt32 num_bits, bool initial_bit = false);

	void clear();
	void *map(void *addr);
	bool read(std::istream *input);
	bool write(std::ostream *output) const;

	void swap(BitVector *target);

private:
	ObjectPool<UInt32> unit_pool_;
	UInt32 num_bits_;

	// Disallows copies.
	BitVector(const BitVector &);
	BitVector &operator=(const BitVector &);
};

}  // namespace sumire

#include "bit-vector-in.h"

#endif  // SUMIRE_BIT_VECTOR_H
