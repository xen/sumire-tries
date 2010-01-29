#ifndef SUMIRE_OBJECT_POOL_IN_H
#define SUMIRE_OBJECT_POOL_IN_H

#include "object-io.h"

#include <cassert>

namespace sumire {

template <typename OBJECT_TYPE>
inline ObjectPool<OBJECT_TYPE>::ObjectPool(UInt32 num_objs_per_block)
	: shift_(0), mask_(0), blocks_(), num_objs_(0)
{
	if (num_objs_per_block == 0)
		num_objs_per_block = DEFAULT_NUM_OBJS_PER_BLOCK;

	shift_ = num_objs_to_shift(num_objs_per_block);
	mask_ = shift_to_mask(shift_);
}

template <typename OBJECT_TYPE>
inline const typename ObjectPool<OBJECT_TYPE>::Object &
ObjectPool<OBJECT_TYPE>::front() const
{
	assert(num_objs_ > 0);

	return (*this)[0];
}

template <typename OBJECT_TYPE>
inline const typename ObjectPool<OBJECT_TYPE>::Object &
ObjectPool<OBJECT_TYPE>::back() const
{
	assert(num_objs_ > 0);

	return (*this)[num_objs_ - 1];
}

template <typename OBJECT_TYPE>
inline typename ObjectPool<OBJECT_TYPE>::Object &
ObjectPool<OBJECT_TYPE>::front()
{
	assert(num_objs_ > 0);

	return (*this)[0];
}

template <typename OBJECT_TYPE>
inline typename ObjectPool<OBJECT_TYPE>::Object &
ObjectPool<OBJECT_TYPE>::back()
{
	assert(num_objs_ > 0);

	return (*this)[num_objs_ - 1];
}

template <typename OBJECT_TYPE>
inline const typename ObjectPool<OBJECT_TYPE>::Object &
ObjectPool<OBJECT_TYPE>::operator[](UInt32 index) const
{
	assert(index < num_objs_);

	return blocks_[index >> shift_][index & mask_];
}

template <typename OBJECT_TYPE>
inline typename ObjectPool<OBJECT_TYPE>::Object &
ObjectPool<OBJECT_TYPE>::operator[](UInt32 index)
{
	assert(index < num_objs_);

	return blocks_[index >> shift_][index & mask_];
}

template <typename OBJECT_TYPE>
inline UInt32 ObjectPool<OBJECT_TYPE>::num_objs_per_block() const
{
	return static_cast<UInt32>(1) << shift_;
}

template <typename OBJECT_TYPE>
inline UInt32 ObjectPool<OBJECT_TYPE>::num_blocks() const
{
	return (num_objs_ + (num_objs_per_block() - 1))
		/ num_objs_per_block();
}

template <typename OBJECT_TYPE>
inline UInt32 ObjectPool<OBJECT_TYPE>::block_size() const
{
	return sizeof(Object) * num_objs_per_block();
}

template <typename OBJECT_TYPE>
inline UInt32 ObjectPool<OBJECT_TYPE>::size() const
{
	UInt32 total_size = blocks_.size();
	if (num_blocks() > 0)
		total_size += blocks_[0].size() * num_blocks();
	return total_size;
}

template <typename OBJECT_TYPE>
inline UInt32 ObjectPool<OBJECT_TYPE>::alloc()
{
	if ((num_objs_ & mask_) == 0)
	{
		UInt32 next_block_id = num_objs_ >> shift_;
		if (next_block_id == blocks_.num_objs())
		{
			ObjectArray<ObjectArray<Object> > blocks;
			blocks.resize((next_block_id != 0) ? (next_block_id * 2) : 1);

			for (UInt32 block_id = 0; block_id < blocks_.num_objs();
				++block_id)
				blocks_[block_id].swap(&blocks[block_id]);
			blocks_.swap(&blocks);
		}
		blocks_[next_block_id].resize(num_objs_per_block());
	}

	return num_objs_++;
}

template <typename OBJECT_TYPE>
inline void ObjectPool<OBJECT_TYPE>::clear()
{
	blocks_.clear();
	num_objs_ = 0;
}

template <typename OBJECT_TYPE>
inline void *ObjectPool<OBJECT_TYPE>::map(void *addr)
{
	assert(addr != NULL);

	ObjectClipper clipper(addr);
	UInt32 num_objs = *clipper.clip<UInt32>();

	return map(clipper.addr(), num_objs);
}

template <typename OBJECT_TYPE>
inline void *ObjectPool<OBJECT_TYPE>::map(void *addr, UInt32 num_objs)
{
	assert(addr != NULL || num_objs == 0);

	ObjectClipper clipper(addr);

	UInt32 num_blocks = (num_objs + (num_objs_per_block() - 1))
		/ num_objs_per_block();
	ObjectArray<ObjectArray<Object> > blocks;
	blocks.resize(num_blocks);

	for (UInt32 block_id = 0; block_id < num_blocks; ++block_id)
	{
		UInt32 num_objs_in_this_block = (block_id + 1 != num_blocks) ?
			num_objs_per_block() : (((num_objs - 1) & mask_) + 1);

		addr = blocks[block_id].map(addr, num_objs_in_this_block);
	}

	clear();
	blocks_.swap(&blocks);
	num_objs_ = num_objs;

	return addr;
}

template <typename OBJECT_TYPE>
inline bool ObjectPool<OBJECT_TYPE>::read(std::istream *input)
{
	assert(input != NULL);

	ObjectReader reader(input);

	UInt32 num_objs;
	if (!reader.read(&num_objs))
		return false;

	UInt32 num_blocks = (num_objs + (num_objs_per_block() - 1))
		/ num_objs_per_block();
	ObjectArray<ObjectArray<Object> > blocks;
	blocks.resize(num_blocks);

	for (UInt32 block_id = 0; block_id < num_blocks; ++block_id)
	{
		blocks[block_id].resize(num_objs_per_block());

		UInt32 num_objs_in_this_block = (block_id + 1 != num_blocks) ?
			num_objs_per_block() : (((num_objs - 1) & mask_) + 1);

		if (!reader.read(blocks[block_id].begin(), num_objs_in_this_block))
			return false;
	}

	clear();
	blocks_.swap(&blocks);
	num_objs_ = num_objs;

	return true;
}

template <typename OBJECT_TYPE>
inline bool ObjectPool<OBJECT_TYPE>::write(std::ostream *output) const
{
	assert(output != NULL);

	ObjectWriter writer(output);

	if (!writer.write(num_objs_))
		return false;

	UInt32 num_blocks = this->num_blocks();
	for (UInt32 block_id = 0; block_id < num_blocks; ++block_id)
	{
		UInt32 num_objs_in_this_block = (block_id + 1 != num_blocks) ?
			num_objs_per_block() : (((num_objs_ - 1) & mask_) + 1);

		if (!writer.write(blocks_[block_id].begin(), num_objs_in_this_block))
			return false;
	}

	return true;
}

template <typename OBJECT_TYPE>
inline UInt32 ObjectPool<OBJECT_TYPE>::num_objs_to_shift(UInt32 num_objs)
{
	UInt32 shift = 0;
	while (num_objs > 0)
	{
		++shift;
		num_objs >>= 1;
	}

	return shift - 1;
}

template <typename OBJECT_TYPE>
inline UInt32 ObjectPool<OBJECT_TYPE>::shift_to_mask(UInt32 shift)
{
	return (static_cast<UInt32>(1) << shift) - 1;
}

template <typename OBJECT_TYPE>
inline void ObjectPool<OBJECT_TYPE>::swap(ObjectPool *target)
{
	assert(target != NULL);

	std::swap(shift_, target->shift_);
	std::swap(mask_, target->mask_);
	blocks_.swap(&target->blocks_);
	std::swap(num_objs_, target->num_objs_);
}

}  // namespace sumire

#endif  // SUMIRE_OBJECT_POOL_IN_H
