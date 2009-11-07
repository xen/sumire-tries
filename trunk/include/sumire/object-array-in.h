#ifndef SUMIRE_OBJECT_ARRAY_IN_H
#define SUMIRE_OBJECT_ARRAY_IN_H

#include "object-io.h"

#include <cassert>

namespace sumire {

// This function discards old objects.
template <typename OBJECT_TYPE>
inline void ObjectArray<OBJECT_TYPE>::resize(UInt32 num_objs)
{
	Object *objs = (num_objs > 0) ? (new Object[num_objs]) : NULL;

	clear();
	objs_ = objs;
	num_objs_ = num_objs;
}

template <typename OBJECT_TYPE>
inline const typename ObjectArray<OBJECT_TYPE>::Object &
ObjectArray<OBJECT_TYPE>::front() const
{
	assert(num_objs_ > 0);

	return objs_[0];
}

template <typename OBJECT_TYPE>
inline const typename ObjectArray<OBJECT_TYPE>::Object &
ObjectArray<OBJECT_TYPE>::back() const
{
	assert(num_objs_ > 0);

	return objs_[num_objs_ - 1];
}

template <typename OBJECT_TYPE>
inline typename ObjectArray<OBJECT_TYPE>::Object &
ObjectArray<OBJECT_TYPE>::front()
{
	assert(num_objs_ > 0);

	return objs_[0];
}

template <typename OBJECT_TYPE>
inline typename ObjectArray<OBJECT_TYPE>::Object &
ObjectArray<OBJECT_TYPE>::back()
{
	assert(num_objs_ > 0);

	return objs_[num_objs_ - 1];
}

template <typename OBJECT_TYPE>
inline const typename ObjectArray<OBJECT_TYPE>::Object &
ObjectArray<OBJECT_TYPE>::operator[](UInt32 index) const
{
	assert(index < num_objs_);

	return objs_[index];
}

template <typename OBJECT_TYPE>
inline typename ObjectArray<OBJECT_TYPE>::Object &
ObjectArray<OBJECT_TYPE>::operator[](UInt32 index)
{
	assert(index < num_objs_);

	return objs_[index];
}

template <typename OBJECT_TYPE>
inline void ObjectArray<OBJECT_TYPE>::clear()
{
	if (needs_delete_)
		delete[] objs_;

	objs_ = NULL;
	num_objs_ = 0;
	needs_delete_ = true;
}

template <typename OBJECT_TYPE>
inline void *ObjectArray<OBJECT_TYPE>::map(void *addr)
{
	assert(addr != NULL);

	ObjectClipper clipper(addr);
	UInt32 num_objs = *clipper.clip<UInt32>();

	return map(clipper.addr(), num_objs);
}

template <typename OBJECT_TYPE>
inline void *ObjectArray<OBJECT_TYPE>::map(void *addr, UInt32 num_objs)
{
	assert(addr != NULL || num_objs == 0);

	clear();

	ObjectClipper clipper(addr);
	objs_ = clipper.clip<Object>(num_objs);
	num_objs_ = num_objs;
	needs_delete_ = false;

	return clipper.addr();
}

template <typename OBJECT_TYPE>
inline bool ObjectArray<OBJECT_TYPE>::read(std::istream *input)
{
	assert(input != NULL);

	ObjectReader reader(input);

	UInt32 num_objs;
	if (!reader.read(&num_objs))
		return false;

	Object *objs = new Object[num_objs];
	if (!reader.read(objs, num_objs))
	{
		delete[] objs;
		return false;
	}

	clear();
	objs_ = objs;
	num_objs_ = num_objs;

	return true;
}

template <typename OBJECT_TYPE>
inline bool ObjectArray<OBJECT_TYPE>::write(std::ostream *output) const
{
	assert(output != NULL);

	ObjectWriter writer(output);

	if (!writer.write(num_objs_))
		return false;

	if (!writer.write(objs_, num_objs_))
		return false;

	return true;
}

template <typename OBJECT_TYPE>
inline void ObjectArray<OBJECT_TYPE>::swap(ObjectArray *target)
{
	assert(target != NULL);

	std::swap(objs_, target->objs_);
	std::swap(num_objs_, target->num_objs_);
	std::swap(needs_delete_, target->needs_delete_);
}

}  // namespace sumire

#endif  // SUMIRE_OBJECT_ARRAY_IN_H
