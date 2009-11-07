#ifndef SUMIRE_OBJECT_WRITER_H
#define SUMIRE_OBJECT_WRITER_H

#include "int-types.h"

#include <cassert>
#include <iostream>

namespace sumire {

class ObjectWriter
{
public:
	explicit ObjectWriter(std::ostream *output) : output_(output) {}

	template <typename OBJECT_TYPE>
	bool write(const OBJECT_TYPE &obj) { return write(&obj, 1); }

	template <typename OBJECT_TYPE>
	bool write(const OBJECT_TYPE *obj);

	template <typename OBJECT_TYPE>
	bool write(const OBJECT_TYPE *obj, UInt32 num_objs)
	{
		assert(obj != NULL || num_objs == 0);

		const char *obj_ptr = reinterpret_cast<const char *>(obj);
		if (!output_->write(obj_ptr, sizeof(OBJECT_TYPE) * num_objs))
			return false;
		return true;
	}

private:
	std::ostream *output_;

	// Disallows copies.
	ObjectWriter(const ObjectWriter &);
	ObjectWriter &operator=(const ObjectWriter &);
};

}  // namespace sumire

#endif  // SUMIRE_OBJECT_WRITER_H
