#ifndef SUMIRE_OBJECT_POOL_H
#define SUMIRE_OBJECT_POOL_H

#include "object-array.h"

#include <iostream>

namespace sumire {

template <typename OBJECT_TYPE>
class ObjectPool
{
public:
	typedef OBJECT_TYPE Object;

	enum { DEFAULT_NUM_OBJS_PER_BLOCK = 1 << 10 };

	explicit ObjectPool(UInt32 num_objs_per_block =
		DEFAULT_NUM_OBJS_PER_BLOCK);
	~ObjectPool() { clear(); }

	const Object &front() const;
	const Object &back() const;
	Object &front();
	Object &back();

	const Object &operator[](UInt32 index) const;
	Object &operator[](UInt32 index);

	UInt32 num_objs_per_block() const;
	UInt32 num_objs() const { return num_objs_; }
	UInt32 num_blocks() const;
	UInt32 block_size() const;
	UInt32 size() const;

	UInt32 alloc();

	void clear();
	void *map(void *addr);
	void *map(void *addr, UInt32 num_objs);
	bool read(std::istream *input);
	bool write(std::ostream *output) const;

	void swap(ObjectPool *target);

private:
	UInt32 shift_;
	UInt32 mask_;
	ObjectArray<ObjectArray<Object> > blocks_;
	UInt32 num_objs_;

	// Disallows copies.
	ObjectPool(const ObjectPool &);
	ObjectPool &operator=(const ObjectPool &);

	static UInt32 num_objs_to_shift(UInt32 num_objs);
	static UInt32 shift_to_mask(UInt32 shift);
};

}  // namespace sumire

#include "object-pool-in.h"

#endif  // SUMIRE_OBJECT_POOL_H
