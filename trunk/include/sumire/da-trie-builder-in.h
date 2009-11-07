#ifndef SUMIRE_DA_TRIE_BUILDER_IN_H
#define SUMIRE_DA_TRIE_BUILDER_IN_H

#include "object-io.h"

#include <cassert>

namespace sumire {

inline bool DaTrieBuilder::build(const TrieBase &trie,
	ObjectArray<DaTrieUnit> *units_ptr)
{
	if (trie.num_nodes() == 0)
		return false;

	clear();
	trie_ = &trie;

	reserve(0);

	temp(0).set_is_used();
	unit(0).set_offset(1);
	unit(0).set_label('\0');

	if (trie_->num_nodes() > 1)
		build_da(trie_->root(), 0);

	fix_all();

	ObjectArray<DaTrieUnit> units;
	units.resize(num_units());
	for (UInt32 index = 0; index < num_units(); ++index)
		units[index] = unit(index);
	units.swap(units_ptr);

	clear();
	return true;
}

inline void DaTrieBuilder::build_da(UInt32 trie_index, UInt32 da_index)
{
	assert(trie_index < trie_->num_units());
	assert(da_index < num_units());

//	if (trie_->label(trie_index) == '\0')
//		return;

	UInt32 offset = arrange_nodes(trie_index, da_index);

	UInt32 trie_child_index = trie_->child(trie_index);
	while (trie_child_index != 0)
	{
		UInt32 da_child_index = offset ^ trie_->label(trie_child_index);
		build_da(trie_child_index, da_child_index);
		trie_child_index = trie_->sibling(trie_child_index);
	}
}

// Arranges child nodes.
inline UInt32 DaTrieBuilder::arrange_nodes(UInt32 trie_index, UInt32 da_index)
{
	assert(trie_index < trie_->num_units());
	assert(da_index < num_units());

	labels_.clear();

	UInt32 value;
	bool has_value = trie_->get_value(trie_index, &value);
	if (has_value)
		labels_.push_back('\0');

	UInt32 trie_child_index = trie_->child(trie_index);
	while (trie_child_index != 0)
	{
		labels_.push_back(trie_->label(trie_child_index));
		trie_child_index = trie_->sibling(trie_child_index);
	}

	UInt32 offset = find_valid_offset(da_index);
	unit(da_index).set_offset(offset);

	if (has_value)
	{
		UInt32 leaf_index = offset;
		reserve(leaf_index);
		unit(leaf_index).set_value(value);
		unit(da_index).set_has_leaf();

		if (labels_.size() > 1)
			unit(da_index).set_child_label(labels_[1]);
	}
	else
		unit(da_index).set_child_label(labels_[0]);

	trie_child_index = trie_->child(trie_index);
	for (UInt32 i = has_value ? 1 : 0; i < labels_.size(); ++i)
	{
		UInt32 da_child_index = offset ^ labels_[i];
		reserve(da_child_index);
		unit(da_child_index).set_label(labels_[i]);

		trie_child_index = trie_->sibling(trie_child_index);
	}
	temp(offset).set_is_used();

	for (UInt32 i = 1; i < labels_.size(); ++i)
	{
		UInt32 src_index = offset ^ labels_[i - 1];
		UInt32 dest_index = offset ^ labels_[i];
		unit(src_index).set_next_sibling(src_index ^ dest_index);
	}

	return offset;
}

inline UInt32 DaTrieBuilder::find_valid_offset(UInt32 index) const
{
	assert(index < num_units());

	if (unfixed_index_ >= num_units())
		return num_units() | (index & 0xFF);

	UInt32 unfixed_index = unfixed_index_;
	do
	{
		UInt32 offset = unfixed_index ^ labels_[0];
		if (is_valid_offset(index, offset))
			return offset;
		unfixed_index = temp(unfixed_index).next();
	} while (unfixed_index != unfixed_index_);

	return num_units() | (index & 0xFF);
}

inline bool DaTrieBuilder::is_valid_offset(UInt32 index, UInt32 offset) const
{
	assert(index < num_units());

	if (temp(offset).is_used())
		return false;

	for (UInt32 i = 1; i < labels_.size(); ++i)
	{
		if (temp(offset ^ labels_[i]).is_fixed())
			return false;
	}

	return true;
}

inline void DaTrieBuilder::fix_all()
{
	UInt32 begin = 0;
	if (num_blocks() > NUM_UNFIXED_BLOCKS)
		begin = num_blocks() - NUM_UNFIXED_BLOCKS;
	UInt32 end = num_blocks();

	for (UInt32 block_id = begin; block_id != end; ++block_id)
		fix_block(block_id);
}

inline void DaTrieBuilder::fix_block(UInt32 block_id)
{
	assert(block_id < num_blocks());

	UInt32 begin = block_id * UNITS_PER_BLOCK;
	UInt32 end = begin + UNITS_PER_BLOCK;

	UInt32 unused_offset_for_label = 0;
	for (UInt32 offset = begin; offset != end; ++offset)
	{
		if (!temp(offset).is_used())
		{
			unused_offset_for_label = offset;
			break;
		}
	}

	for (UInt32 index = begin; index != end; ++index)
	{
		if (!temp(index).is_fixed())
		{
			reserve(index);
			unit(index).set_label(index ^ unused_offset_for_label);
		}
	}
}

void DaTrieBuilder::reserve(UInt32 index)
{
	if (index >= num_units())
		expand();

	if (index == unfixed_index_)
	{
		unfixed_index_ = temp(index).next();
		if (unfixed_index_ == index)
			unfixed_index_ = num_units();
	}
	temp(temp(index).prev()).set_next(temp(index).next());
	temp(temp(index).next()).set_prev(temp(index).prev());
	temp(index).set_is_fixed();
}

inline void DaTrieBuilder::expand()
{
	UInt32 src_num_units = num_units();
	UInt32 src_num_blocks = num_blocks();

	UInt32 dest_num_units = src_num_units + UNITS_PER_BLOCK;
	UInt32 dest_num_blocks = src_num_blocks + 1;

	if (dest_num_blocks > NUM_UNFIXED_BLOCKS)
		fix_block(src_num_blocks - NUM_UNFIXED_BLOCKS);

	while (num_units() < dest_num_units)
		unit_pool_.alloc();
	temp_blocks_.resize(dest_num_blocks, NULL);

	if (dest_num_blocks > NUM_UNFIXED_BLOCKS)
	{
		UInt32 block_id = src_num_blocks - NUM_UNFIXED_BLOCKS;
		std::swap(temp_blocks_[block_id], temp_blocks_.back());
		for (UInt32 i = src_num_units; i < dest_num_units; ++i)
			temp(i).clear();
	}
	else
		temp_blocks_.back() = new DaTrieTempUnit[UNITS_PER_BLOCK];

	for (UInt32 i = src_num_units + 1; i < dest_num_units; ++i)
	{
		temp(i - 1).set_next(i);
		temp(i).set_prev(i - 1);
	}

	temp(src_num_units).set_prev(dest_num_units - 1);
	temp(dest_num_units - 1).set_next(src_num_units);

	temp(src_num_units).set_prev(temp(unfixed_index_).prev());
	temp(dest_num_units - 1).set_next(unfixed_index_);

	temp(temp(unfixed_index_).prev()).set_next(src_num_units);
	temp(unfixed_index_).set_prev(dest_num_units - 1);
}

inline const DaTrieUnit &DaTrieBuilder::unit(UInt32 index) const
{
	assert(index < num_units());

	return unit_pool_[index];
}

inline const DaTrieTempUnit &DaTrieBuilder::temp(UInt32 index) const
{
	assert((index / UNITS_PER_BLOCK) < num_blocks());

	UInt32 block_id = index / UNITS_PER_BLOCK;
	UInt32 unit_id = index % UNITS_PER_BLOCK;

	return temp_blocks_[block_id][unit_id];
}

inline DaTrieUnit &DaTrieBuilder::unit(UInt32 index)
{
	assert(index < num_units());

	return unit_pool_[index];
}

inline DaTrieTempUnit &DaTrieBuilder::temp(UInt32 index)
{
	assert((index / UNITS_PER_BLOCK) < num_blocks());

	UInt32 block_id = index / UNITS_PER_BLOCK;
	UInt32 unit_id = index % UNITS_PER_BLOCK;

	return temp_blocks_[block_id][unit_id];
}

inline void DaTrieBuilder::clear()
{
	for (UInt32 block_id = 0; block_id < num_blocks(); ++block_id)
		delete[] temp_blocks_[block_id];

	trie_ = NULL;
	unit_pool_.clear();
	std::vector<DaTrieTempUnit *>(0).swap(temp_blocks_);
	std::vector<UInt8>(0).swap(labels_);
	unfixed_index_ = 0;
}

}  // namespace

#endif  // SUMIRE_DA_TRIE_BUILDER_IN_H
