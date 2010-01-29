#ifndef SUMIRE_BIT_VECTOR_IN_H
#define SUMIRE_BIT_VECTOR_IN_H

#include "object-io.h"

#include <cassert>

namespace sumire {

BitVector::Accessor &BitVector::Accessor::operator=(bool bit)
{
	bv_->set(index_, bit);
	return *this;
}

inline bool BitVector::get(UInt32 index) const
{
	assert(index < num_bits());

	const UInt32 unit_id = index / BITS_PER_UNIT;
	const UInt32 bit_id = index % BITS_PER_UNIT;
	return (unit_pool_[unit_id] & (UNIT_1 << bit_id)) != 0;
}

inline void BitVector::set(UInt32 index, bool bit)
{
	assert(index < num_bits());

	const UInt32 unit_id = index / BITS_PER_UNIT;
	const UInt32 bit_id = index % BITS_PER_UNIT;
	if (bit)
		unit_pool_[unit_id] |= (UNIT_1 << bit_id);
	else
		unit_pool_[unit_id] &= ~(UNIT_1 << bit_id);
}

inline void BitVector::add(bool bit)
{
	if (num_bits_ == capacity())
		unit_pool_.alloc();
	++num_bits_;

	set(num_bits_ - 1, bit);
}

inline UInt32 BitVector::unit(UInt32 unit_id) const
{
	assert(unit_id < num_units());

	return unit_pool_[unit_id];
}

inline void BitVector::clear()
{
	unit_pool_.clear();
	num_bits_ = 0;
}

inline void *BitVector::map(void *addr)
{
	assert(addr != NULL);

	addr = unit_pool_.map(addr);

	ObjectClipper clipper(addr);
	num_bits_ = *clipper.clip<UInt32>();

	return clipper.addr();
}

inline bool BitVector::read(std::istream *input)
{
	assert(input != NULL);

	ObjectPool<UInt32> unit_pool;
	if (!unit_pool.read(input))
		return false;

	ObjectReader reader(input);

	UInt32 num_bits;
	if (!reader.read(&num_bits))
		return false;

	clear();
	unit_pool_.swap(&unit_pool);
	num_bits_ = num_bits;

	return true;
}

inline bool BitVector::write(std::ostream *output) const
{
	assert(output != NULL);

	if (!unit_pool_.write(output))
		return false;

	ObjectWriter writer(output);

	if (!writer.write(num_bits_))
		return false;

	return true;
}

inline void BitVector::swap(BitVector *target)
{
	assert(target != NULL);

	unit_pool_.swap(&target->unit_pool_);
	std::swap(num_bits_, target->num_bits_);
}

}  // namespace sumire

#endif  // SUMIRE_BIT_VECTOR_IN_H
