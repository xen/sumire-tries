#ifndef SUMIRE_BASIC_SUCCINCT_BIT_VECTOR_IN_H
#define SUMIRE_BASIC_SUCCINCT_BIT_VECTOR_IN_H

#include "object-io.h"

#include <cassert>

namespace sumire {

inline BasicSuccinctBitVector::Block::Block()
	: global_rank_(0), local_ranks_()
{
	for (UInt32 unit_id = 0; unit_id < UNITS_PER_BLOCK; ++unit_id)
		local_ranks_[unit_id] = 0;
}

inline void BasicSuccinctBitVector::Block::set_rank(
	UInt32 local_id, UInt32 rank)
{
	if (local_id == 0)
		global_rank_ = rank;

	local_ranks_[local_id] = rank - global_rank_;
}

inline UInt32 BasicSuccinctBitVector::Block::rank(UInt32 local_id) const
{
	assert(local_id < UNITS_PER_BLOCK);

	return global_rank_ + local_ranks_[local_id];
}

inline UInt32 BasicSuccinctBitVector::Block::local_rank(UInt32 local_id) const
{
	assert(local_id < UNITS_PER_BLOCK);

	return local_ranks_[local_id];
}

inline void BasicSuccinctBitVector::build(const BitVector &bv)
{
	UInt32 num_blocks = (bv.num_units() + UNITS_PER_BLOCK - 1)
		/ UNITS_PER_BLOCK;

	ObjectArray<Block> blocks;
	blocks.resize(num_blocks);
	ObjectArray<UInt32> units;
	units.resize(bv.num_units());

	UInt32 num_ones = 0;
	for (UInt32 unit_id = 0; unit_id < bv.num_units(); ++unit_id)
	{
		units[unit_id] = bv.unit(unit_id);

		UInt32 block_id = unit_id / UNITS_PER_BLOCK;
		UInt32 local_id = unit_id % UNITS_PER_BLOCK;

		blocks[block_id].set_rank(local_id, num_ones);

		UInt32 left_bits = bv.num_bits() - unit_id * BITS_PER_UNIT;
		UInt32 left_shift = 0;
		if (left_bits < BITS_PER_UNIT)
			left_shift = BITS_PER_UNIT - left_bits;

		num_ones += pop_count(bv.unit(unit_id) << left_shift) >> 24;
	}

	for (UInt32 unit_id = bv.num_units();
		unit_id < num_blocks * UNITS_PER_BLOCK; ++unit_id)
	{
		UInt32 block_id = unit_id / UNITS_PER_BLOCK;
		UInt32 local_id = unit_id % UNITS_PER_BLOCK;

		blocks[block_id].set_rank(local_id,
			blocks[block_id].rank(0) + BITS_PER_BLOCK - 1);
	}

	clear();
	blocks_.swap(&blocks);
	units_.swap(&units);
	num_bits_ = bv.num_bits();
	num_ones_ = num_ones;
}

inline UInt32 BasicSuccinctBitVector::rank_1(UInt32 index) const
{
	assert(index < num_bits());

	UInt32 unit_id = index / BITS_PER_UNIT;
	UInt32 block_id = unit_id / UNITS_PER_BLOCK;
	UInt32 local_id = unit_id % UNITS_PER_BLOCK;
	UInt32 bit_id = index % BITS_PER_UNIT;

	return blocks_[block_id].rank(local_id) + (pop_count(units_[unit_id] &
		(~UNIT_0 >> (BITS_PER_UNIT - bit_id - 1))) >> 24);
}

inline UInt32 BasicSuccinctBitVector::rank_0(UInt32 index) const
{
	assert(index < num_bits());

	return index - rank_1(index) + 1;
}

inline UInt32 BasicSuccinctBitVector::select_1(UInt32 count) const
{
	assert(count > 0);
	assert(count <= num_ones());

	UInt32 left = 0;
	UInt32 right = num_blocks();
	while (left + 1 < right)
	{
		UInt32 middle = (left + right) / 2;
		if (blocks_[middle].global_rank() >= count)
			right = middle;
		else
			left = middle;
	}
	UInt32 global_id = left;
	count -= blocks_[global_id].global_rank();

	UInt32 local_id = 1;
	for ( ; local_id < UNITS_PER_BLOCK; ++local_id)
	{
		if (blocks_[global_id].local_rank(local_id) >= count)
			break;
	}
	--local_id;
	count -= blocks_[global_id].local_rank(local_id);

	UInt32 index = (global_id * BITS_PER_BLOCK) + (local_id * BITS_PER_UNIT);
	UInt32 unit = units_[index / BITS_PER_UNIT];
	UInt32 bytes = pop_count(unit);

	UInt64 offset = 0;
	if (count <= ((bytes << 16) >> 24))
		offset += 16;
	if (count <= ((bytes << (offset + 8)) >> 24))
		offset += 8;

	count -= ((bytes << offset) << 8) >> 24;
	unit >>= BITS_PER_UNIT - (offset + 8);
	index += BITS_PER_UNIT - (offset + 8);

	while (count > 0)
	{
		if (unit & 1)
			--count;
		unit /= 2;
		++index;
	}

	return index - 1;
}

inline UInt32 BasicSuccinctBitVector::select_0(UInt32 count) const
{
	assert(count > 0);
	assert(count <= num_zeros());

	UInt32 left = 0;
	UInt32 right = num_blocks();
	while (left + 1 < right)
	{
		UInt32 middle = (left + right) / 2;
		if (middle * BITS_PER_BLOCK - blocks_[middle].global_rank() >= count)
			right = middle;
		else
			left = middle;
	}
	UInt32 global_id = left;
	count -= left * BITS_PER_BLOCK - blocks_[left].global_rank();

	UInt32 local_id = 1;
	for ( ; local_id < UNITS_PER_BLOCK; ++local_id)
	{
		UInt32 local_rank = local_id * BITS_PER_UNIT
			- blocks_[global_id].local_rank(local_id);
		if (local_rank >= count)
			break;
	}
	--local_id;
	count -= local_id * BITS_PER_UNIT
		- blocks_[global_id].local_rank(local_id);

	UInt32 index = (global_id * BITS_PER_BLOCK) + (local_id * BITS_PER_UNIT);
	UInt32 unit = units_[index / BITS_PER_UNIT];
	UInt32 bytes = pop_count(~unit);

	UInt64 offset = 0;
	if (count <= ((bytes << 16) >> 24))
		offset += 16;
	if (count <= ((bytes << (offset + 8)) >> 24))
		offset += 8;

	count -= ((bytes << offset) << 8) >> 24;
	unit >>= BITS_PER_UNIT - (offset + 8);
	index += BITS_PER_UNIT - (offset + 8);

	while (count > 0)
	{
		if (~unit & 1)
			--count;
		unit /= 2;
		++index;
	}

	return index - 1;
}

inline bool BasicSuccinctBitVector::operator[](UInt32 index) const
{
	assert(index < num_bits());

	UInt32 unit_id = index / BITS_PER_UNIT;
	UInt32 bit_id = index % BITS_PER_UNIT;

	return (units_[unit_id] & (UNIT_1 << bit_id)) != 0;
}

inline bool BasicSuccinctBitVector::get(UInt32 index) const
{
	assert(index < num_bits());

	UInt32 unit_id = index / BITS_PER_UNIT;
	UInt32 bit_id = index % BITS_PER_UNIT;

	return (units_[unit_id] & (UNIT_1 << bit_id)) != 0;
}

inline void BasicSuccinctBitVector::clear()
{
	blocks_.clear();
	units_.clear();
	num_bits_ = 0;
	num_ones_ = 0;
}

inline void *BasicSuccinctBitVector::map(void *addr)
{
	assert(addr != NULL);

	clear();

	addr = blocks_.map(addr);
	addr = units_.map(addr);

	ObjectClipper clipper(addr);
	num_bits_ = *clipper.clip<UInt32>();
	num_ones_ = *clipper.clip<UInt32>();
	addr = clipper.addr();

	return addr;
}

inline bool BasicSuccinctBitVector::read(std::istream *input)
{
	assert(input != NULL);

	ObjectArray<Block> blocks;
	if (!blocks.read(input))
		return false;

	ObjectArray<UInt32> units;
	if (!units.read(input))
		return false;

	ObjectReader reader(input);

	UInt32 num_bits, num_ones;
	if (!reader.read(&num_bits) || !reader.read(&num_ones))
		return false;

	clear();
	blocks_.swap(&blocks);
	units_.swap(&units);
	num_bits_ = num_bits;
	num_ones_ = num_ones;

	return true;
}

inline bool BasicSuccinctBitVector::write(std::ostream *output) const
{
	assert(output != NULL);

	if (!blocks_.write(output) || !units_.write(output))
		return false;

	ObjectWriter writer(output);
	if (!writer.write(num_bits_) || !writer.write(num_ones_))
		return false;

	return true;
}

inline void BasicSuccinctBitVector::swap(BasicSuccinctBitVector *target)
{
	assert(target != NULL);

	blocks_.swap(&target->blocks_);
	units_.swap(&target->units_);
	std::swap(num_bits_, target->num_bits_);
	std::swap(num_ones_, target->num_ones_);
}

inline UInt32 BasicSuccinctBitVector::pop_count(UInt32 unit)
{
	unit = ((unit & 0xAAAAAAAA) >> 1) + (unit & 0x55555555);
	unit = ((unit & 0xCCCCCCCC) >> 2) + (unit & 0x33333333);
	unit = ((unit >> 4) + unit) & 0x0F0F0F0F;
	unit += unit << 8;
	unit += unit << 16;
	return unit;
}

}  // namespace sumire

#endif  // SUMIRE_BASIC_SUCCINCT_BIT_VECTOR_IN_H
