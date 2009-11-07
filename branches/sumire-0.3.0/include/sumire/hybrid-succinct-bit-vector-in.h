#ifndef SUMIRE_HYBRID_SUCCINCT_BIT_VECTOR_IN_H
#define SUMIRE_HYBRID_SUCCINCT_BIT_VECTOR_IN_H

#include "object-io.h"

#include <cassert>

namespace sumire {

inline HybridSuccinctBitVector::RankBlock::RankBlock()
	: global_rank_(0), local_ranks_()
{
	for (UInt32 unit_id = 0; unit_id < UNITS_PER_BLOCK; ++unit_id)
		local_ranks_[unit_id] = 0;
}

inline void HybridSuccinctBitVector::RankBlock::set_rank(
	UInt32 local_id, UInt32 rank)
{
	if (local_id == 0)
		global_rank_ = rank;

	local_ranks_[local_id] = rank - global_rank_;
}

inline UInt32 HybridSuccinctBitVector::RankBlock::rank(UInt32 local_id) const
{
	assert(local_id < UNITS_PER_BLOCK);

	return global_rank_ + local_ranks_[local_id];
}

inline UInt32 HybridSuccinctBitVector::RankBlock::local_rank(
	UInt32 local_id) const
{
	assert(local_id < UNITS_PER_BLOCK);

	return local_ranks_[local_id];
}

inline void HybridSuccinctBitVector::build(const BitVector &bv)
{
	UInt32 num_blocks = (bv.num_units() + UNITS_PER_BLOCK - 1)
		/ UNITS_PER_BLOCK;

	ObjectArray<RankBlock> rank_blocks;
	ObjectArray<UInt32> units;
	rank_blocks.resize(num_blocks);
	units.resize(bv.num_units());

	UInt32 num_ones = 0;
	for (UInt32 unit_id = 0; unit_id < bv.num_units(); ++unit_id)
	{
		units[unit_id] = bv.unit(unit_id);

		UInt32 block_id = unit_id / UNITS_PER_BLOCK;
		UInt32 local_id = unit_id % UNITS_PER_BLOCK;

		rank_blocks[block_id].set_rank(local_id, num_ones);

		UInt32 left_bits = bv.num_bits() - unit_id * BITS_PER_UNIT;
		UInt32 left_shift = 0;
		if (left_bits < BITS_PER_UNIT)
			left_shift = BITS_PER_UNIT - left_bits;

		num_ones += pop_count(bv.unit(unit_id) << left_shift);
	}
	UInt32 num_zeros = bv.num_bits() - num_ones;

	for (UInt32 unit_id = bv.num_units();
		unit_id < num_blocks * UNITS_PER_BLOCK; ++unit_id)
	{
		UInt32 block_id = unit_id / UNITS_PER_BLOCK;
		UInt32 local_id = unit_id % UNITS_PER_BLOCK;

		rank_blocks[block_id].set_rank(local_id,
			rank_blocks[block_id].rank(0) + BITS_PER_BLOCK - 1);
	}

	ObjectArray<UInt32> select_1s, select_0s;
	build_select(rank_blocks, num_ones, num_zeros, &select_1s, &select_0s);

	clear();
	rank_blocks_.swap(&rank_blocks);
	select_1s_.swap(&select_1s);
	select_0s_.swap(&select_0s);
	units_.swap(&units);
	num_bits_ = bv.num_bits();
	num_ones_ = num_ones;
}

inline UInt32 HybridSuccinctBitVector::rank_1(UInt32 index) const
{
	assert(index < num_bits());

	UInt32 unit_id = index / BITS_PER_UNIT;
	UInt32 block_id = unit_id / UNITS_PER_BLOCK;
	UInt32 local_id = unit_id % UNITS_PER_BLOCK;
	UInt32 bit_id = index % BITS_PER_UNIT;

	return rank_blocks_[block_id].rank(local_id) + pop_count(units_[unit_id] &
		(~UNIT_0 >> (BITS_PER_UNIT - bit_id - 1)));
}

inline UInt32 HybridSuccinctBitVector::rank_0(UInt32 index) const
{
	assert(index < num_bits());

	return index - rank_1(index) + 1;
}

inline UInt32 HybridSuccinctBitVector::select_1(UInt32 count) const
{
	assert(count > 0);
	assert(count <= num_ones());

	UInt32 left = select_1s_[(count - 1) / BITS_PER_BLOCK];
	UInt32 right = select_1s_[((count - 1) / BITS_PER_BLOCK) + 1] + 1;
	while (left + 1 < right)
	{
		UInt32 middle = (left + right) / 2;
		if (rank_blocks_[middle].global_rank() >= count)
			right = middle;
		else
			left = middle;
	}
	UInt32 global_id = left;
	count -= rank_blocks_[global_id].global_rank();

	UInt32 local_id = 1;
	for ( ; local_id < UNITS_PER_BLOCK; ++local_id)
	{
		if (rank_blocks_[global_id].local_rank(local_id) >= count)
			break;
	}
	--local_id;
	count -= rank_blocks_[global_id].local_rank(local_id);

	UInt32 index = (global_id * BITS_PER_BLOCK) + (local_id * BITS_PER_UNIT);
	UInt32 unit = units_[index / BITS_PER_UNIT];
	for (int div = 2; div <= BITS_PER_UNIT / 8; div *= 2)
	{
		UInt32 num_bits = BITS_PER_UNIT / div;
		UInt32 num_ones = pop_count(unit & ((UNIT_1 << num_bits) - 1));
		if (num_ones < count)
		{
			unit >>= num_bits;
			index += num_bits;
			count -= num_ones;
		}
	}

	while (count > 0)
	{
		if (unit & 1)
			--count;
		unit /= 2;
		++index;
	}

	return index - 1;
}

inline UInt32 HybridSuccinctBitVector::select_0(UInt32 count) const
{
	assert(count > 0);
	assert(count <= num_zeros());

	UInt32 left = select_0s_[(count - 1) / BITS_PER_BLOCK];
	UInt32 right = select_0s_[((count - 1) / BITS_PER_BLOCK) + 1] + 1;
	while (left + 1 < right)
	{
		UInt32 middle = (left + right) / 2;
		if (middle * BITS_PER_BLOCK
			- rank_blocks_[middle].global_rank() >= count)
			right = middle;
		else
			left = middle;
	}
	UInt32 global_id = left;
	count -= left * BITS_PER_BLOCK - rank_blocks_[left].global_rank();

	UInt32 local_id = 1;
	for ( ; local_id < UNITS_PER_BLOCK; ++local_id)
	{
		UInt32 local_rank = local_id * BITS_PER_UNIT
			- rank_blocks_[global_id].local_rank(local_id);
		if (local_rank >= count)
			break;
	}
	--local_id;
	count -= local_id * BITS_PER_UNIT
		- rank_blocks_[global_id].local_rank(local_id);

	UInt32 index = (global_id * BITS_PER_BLOCK) + (local_id * BITS_PER_UNIT);
	UInt32 unit = units_[index / BITS_PER_UNIT];
	for (int div = 2; div <= BITS_PER_UNIT / 8; div *= 2)
	{
		UInt32 num_bits = BITS_PER_UNIT / div;
		UInt32 num_zeros = num_bits
			- pop_count(unit & ((UNIT_1 << num_bits) - 1));
		if (num_zeros < count)
		{
			unit >>= num_bits;
			index += num_bits;
			count -= num_zeros;
		}
	}

	while (count > 0)
	{
		if (~unit & 1)
			--count;
		unit /= 2;
		++index;
	}

	return index - 1;
}

inline bool HybridSuccinctBitVector::operator[](UInt32 index) const
{
	assert(index < num_bits());

	UInt32 unit_id = index / BITS_PER_UNIT;
	UInt32 bit_id = index % BITS_PER_UNIT;

	return (units_[unit_id] & (UNIT_1 << bit_id)) != 0;
}

inline bool HybridSuccinctBitVector::get(UInt32 index) const
{
	assert(index < num_bits());

	UInt32 unit_id = index / BITS_PER_UNIT;
	UInt32 bit_id = index % BITS_PER_UNIT;

	return (units_[unit_id] & (UNIT_1 << bit_id)) != 0;
}

inline UInt32 HybridSuccinctBitVector::size() const
{
	return rank_blocks_.size() + select_1s_.size()
		+ select_0s_.size() + units_.size();
}

inline void HybridSuccinctBitVector::clear()
{
	rank_blocks_.clear();
	select_1s_.clear();
	select_0s_.clear();
	units_.clear();
	num_bits_ = 0;
	num_ones_ = 0;
}

inline void *HybridSuccinctBitVector::map(void *addr)
{
	assert(addr != NULL);

	clear();

	addr = rank_blocks_.map(addr);
	addr = select_1s_.map(addr);
	addr = select_0s_.map(addr);
	addr = units_.map(addr);

	ObjectClipper clipper(addr);
	num_bits_ = *clipper.clip<UInt32>();
	num_ones_ = *clipper.clip<UInt32>();
	addr = clipper.addr();

	return addr;
}

inline bool HybridSuccinctBitVector::read(std::istream *input)
{
	assert(input != NULL);

	ObjectArray<RankBlock> rank_blocks;
	if (!rank_blocks.read(input))
		return false;

	ObjectArray<UInt32> select_1s, select_0s;
	if (!select_1s.read(input) || !select_0s.read(input))
		return false;

	ObjectArray<UInt32> units;
	if (!units.read(input))
		return false;

	ObjectReader reader(input);

	UInt32 num_bits, num_ones;
	if (!reader.read(&num_bits) || !reader.read(&num_ones))
		return false;

	clear();
	rank_blocks_.swap(&rank_blocks);
	select_1s_.swap(&select_1s);
	select_0s_.swap(&select_0s);
	units_.swap(&units);
	num_bits_ = num_bits;
	num_ones_ = num_ones;

	return true;
}

inline bool HybridSuccinctBitVector::write(std::ostream *output) const
{
	assert(output != NULL);

	if (!rank_blocks_.write(output) || !select_1s_.write(output) ||
		!select_0s_.write(output) || !units_.write(output))
		return false;

	ObjectWriter writer(output);
	if (!writer.write(num_bits_) || !writer.write(num_ones_))
		return false;

	return true;
}

inline void HybridSuccinctBitVector::swap(HybridSuccinctBitVector *target)
{
	assert(target != NULL);

	rank_blocks_.swap(&target->rank_blocks_);
	select_1s_.swap(&target->select_1s_);
	select_0s_.swap(&target->select_0s_);
	units_.swap(&target->units_);
	std::swap(num_bits_, target->num_bits_);
	std::swap(num_ones_, target->num_ones_);
}

inline void HybridSuccinctBitVector::build_select(
	const ObjectArray<RankBlock> &rank_blocks,
	UInt32 num_ones, UInt32 num_zeros,
	ObjectArray<UInt32> *select_1s, ObjectArray<UInt32> *select_0s)
{
	UInt32 num_blocks = rank_blocks.num_objs();

	select_1s->resize(((num_ones + BITS_PER_BLOCK - 1) / BITS_PER_BLOCK) + 1);
	select_0s->resize(((num_zeros + BITS_PER_BLOCK - 1) / BITS_PER_BLOCK) + 1);
	select_1s->front() = 0;
	select_0s->front() = 0;
	select_1s->back() = (num_blocks > 0) ? (num_blocks - 1) : 0;
	select_0s->back() = (num_blocks > 0) ? (num_blocks - 1) : 0;

	UInt32 threshold_1 = 0;
	UInt32 threshold_0 = 0;
	for (UInt32 block_id = 1; block_id < num_blocks; ++block_id)
	{
		UInt32 rank_1 = rank_blocks[block_id].global_rank();
		UInt32 rank_0 = (block_id * BITS_PER_BLOCK) - rank_1;

		if (rank_1 > threshold_1)
		{
			(*select_1s)[(rank_1 - 1) / BITS_PER_BLOCK] = block_id - 1;
			threshold_1 += BITS_PER_BLOCK;
		}
		if (rank_0 > threshold_0)
		{
			(*select_0s)[(rank_0 - 1) / BITS_PER_BLOCK] = block_id - 1;
			threshold_0 += BITS_PER_BLOCK;
		}
	}
	if (num_ones > threshold_1)
		(*select_1s)[num_ones / BITS_PER_BLOCK] = num_blocks - 1;
	if (num_zeros > threshold_0)
		(*select_0s)[num_zeros / BITS_PER_BLOCK] = num_blocks - 1;
}

inline UInt32 HybridSuccinctBitVector::pop_count(UInt32 unit)
{
	unit = ((unit & 0xAAAAAAAA) >> 1) + (unit & 0x55555555);
	unit = ((unit & 0xCCCCCCCC) >> 2) + (unit & 0x33333333);
	unit = ((unit >> 4) + unit) & 0x0F0F0F0F;
	unit = (unit >> 8) + unit;
	return ((unit >> 16) + unit) & 0x3F;
}

}  // namespace sumire

#endif  // SUMIRE_HYBRID_SUCCINCT_BIT_VECTOR_IN_H
