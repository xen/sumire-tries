#ifndef SUMIRE_SIMPLIFIED_SUCCINCT_BIT_VECTOR_IN_H
#define SUMIRE_SIMPLIFIED_SUCCINCT_BIT_VECTOR_IN_H

#include "object-io.h"
#include "select-table.h"

#include <cassert>

namespace sumire {

inline void SimplifiedSuccinctBitVector::build(const BitVector &bv)
{
	ObjectArray<Unit> units;
	units.resize(bv.num_units());

	UInt32 num_ones = 0;
	for (UInt32 unit_id = 0; unit_id < bv.num_units(); ++unit_id)
	{
		units[unit_id].set_rank(num_ones);
		units[unit_id].set_value(bv.unit(unit_id));

		UInt32 left_bits = bv.num_bits() - unit_id * BITS_PER_UNIT;
		UInt32 left_shift = 0;
		if (left_bits < BITS_PER_UNIT)
			left_shift = BITS_PER_UNIT - left_bits;

		UInt32 count = pop_count(bv.unit(unit_id) << left_shift) >> 24;
		num_ones += count;
	}

	clear();
	units_.swap(&units);
	num_bits_ = bv.num_bits();
	num_ones_ = num_ones;
}

inline UInt32 SimplifiedSuccinctBitVector::rank_1(UInt32 index) const
{
	assert(index < num_bits());

	UInt32 unit_id = index / BITS_PER_UNIT;
	UInt32 bit_id = index % BITS_PER_UNIT;

	return units_[unit_id].rank() + (pop_count(units_[unit_id].value() &
		(~UNIT_0 >> (BITS_PER_UNIT - bit_id - 1))) >> 24);
}

inline UInt32 SimplifiedSuccinctBitVector::rank_0(UInt32 index) const
{
	assert(index < num_bits());

	return index - rank_1(index) + 1;
}

inline UInt32 SimplifiedSuccinctBitVector::select_1(UInt32 count) const
{
	assert(count > 0);
	assert(count <= num_ones());

	UInt32 left = 0;
	UInt32 right = num_units();
	while (left + 1 < right)
	{
		UInt32 middle = (left + right) / 2;
		if (units_[middle].rank() >= count)
			right = middle;
		else
			left = middle;
	}
	UInt32 unit_id = left;
	count -= units_[unit_id].rank();

	UInt32 index = unit_id * BITS_PER_UNIT;
	UInt32 unit = units_[unit_id].value();
	UInt32 bytes = pop_count(unit);

	UInt64 offset = 0;
	if (count <= ((bytes << 16) >> 24))
		offset += 16;
	if (count <= ((bytes << (offset + 8)) >> 24))
		offset += 8;

	count -= ((bytes << offset) << 8) >> 24;
	unit >>= BITS_PER_UNIT - (offset + 8);
	index += BITS_PER_UNIT - (offset + 8);

	index += SelectTable::lookup(true, count, unit);

	return index - 1;
}

inline UInt32 SimplifiedSuccinctBitVector::select_0(UInt32 count) const
{
	assert(count > 0);
	assert(count <= num_zeros());

	UInt32 left = 0;
	UInt32 right = num_units();
	while (left + 1 < right)
	{
		UInt32 middle = (left + right) / 2;
		if ((middle * BITS_PER_UNIT) - units_[middle].rank() >= count)
			right = middle;
		else
			left = middle;
	}
	UInt32 unit_id = left;
	count -= (unit_id * BITS_PER_UNIT) - units_[unit_id].rank();

	UInt32 index = unit_id * BITS_PER_UNIT;
	UInt32 unit = units_[unit_id].value();
	UInt32 bytes = pop_count(~unit);

	UInt64 offset = 0;
	if (count <= ((bytes << 16) >> 24))
		offset += 16;
	if (count <= ((bytes << (offset + 8)) >> 24))
		offset += 8;

	count -= ((bytes << offset) << 8) >> 24;
	unit >>= BITS_PER_UNIT - (offset + 8);
	index += BITS_PER_UNIT - (offset + 8);

	index += SelectTable::lookup(false, count, unit);

	return index - 1;
}

inline bool SimplifiedSuccinctBitVector::operator[](UInt32 index) const
{
	assert(index < num_bits());

	UInt32 unit_id = index / BITS_PER_UNIT;
	UInt32 bit_id = index % BITS_PER_UNIT;

	return (units_[unit_id].value() & (UNIT_1 << bit_id)) != 0;
}

inline bool SimplifiedSuccinctBitVector::get(UInt32 index) const
{
	assert(index < num_bits());

	UInt32 unit_id = index / BITS_PER_UNIT;
	UInt32 bit_id = index % BITS_PER_UNIT;

	return (units_[unit_id].value() & (UNIT_1 << bit_id)) != 0;
}

inline void SimplifiedSuccinctBitVector::clear()
{
	units_.clear();
	num_bits_ = 0;
	num_ones_ = 0;
}

inline void *SimplifiedSuccinctBitVector::map(void *addr)
{
	assert(addr != NULL);

	clear();

	addr = units_.map(addr);

	ObjectClipper clipper(addr);
	num_bits_ = *clipper.clip<UInt32>();
	num_ones_ = *clipper.clip<UInt32>();
	addr = clipper.addr();

	return addr;
}

inline bool SimplifiedSuccinctBitVector::read(std::istream *input)
{
	assert(input != NULL);

	ObjectArray<Unit> units;
	if (!units.read(input))
		return false;

	ObjectReader reader(input);

	UInt32 num_bits, num_ones;
	if (!reader.read(&num_bits) || !reader.read(&num_ones))
		return false;

	clear();
	units_.swap(&units);
	num_bits_ = num_bits;
	num_ones_ = num_ones;

	return true;
}

inline bool SimplifiedSuccinctBitVector::write(std::ostream *output) const
{
	assert(output != NULL);

	if (!units_.write(output))
		return false;

	ObjectWriter writer(output);
	if (!writer.write(num_bits_) || !writer.write(num_ones_))
		return false;

	return true;
}

inline void SimplifiedSuccinctBitVector::swap(
	SimplifiedSuccinctBitVector *target)
{
	assert(target != NULL);

	units_.swap(&target->units_);
	std::swap(num_bits_, target->num_bits_);
	std::swap(num_ones_, target->num_ones_);
}

inline UInt32 SimplifiedSuccinctBitVector::pop_count(UInt32 unit)
{
	unit = ((unit & 0xAAAAAAAA) >> 1) + (unit & 0x55555555);
	unit = ((unit & 0xCCCCCCCC) >> 2) + (unit & 0x33333333);
	unit = ((unit >> 4) + unit) & 0x0F0F0F0F;
	unit += unit << 8;
	unit += unit << 16;
	return unit;
}

}  // namespace sumire

#endif  // SUMIRE_SIMPLIFIED_SUCCINCT_BIT_VECTOR_IN_H
