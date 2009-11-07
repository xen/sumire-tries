#ifndef SUMIRE_OBJECT_CLIPPER_H
#define SUMIRE_OBJECT_CLIPPER_H

#include "int-types.h"

#include <cassert>

namespace sumire {

class ObjectClipper
{
public:
	explicit ObjectClipper(void *addr) : addr_(addr) {}

	template <typename OBJECT_TYPE>
	OBJECT_TYPE *clip(UInt32 num_objs = 1)
	{
		assert(addr_ != NULL);

		OBJECT_TYPE *obj_ptr = reinterpret_cast<OBJECT_TYPE *>(addr_);
		addr_ = obj_ptr + num_objs;
		return obj_ptr;
	}

	void *addr() { return addr_; }

private:
	void *addr_;

	// Disallows copies.
	ObjectClipper(const ObjectClipper &);
	ObjectClipper &operator=(const ObjectClipper &);
};

}  // namespace sumire

#endif  // SUMIRE_OBJECT_CLIPPER_H
