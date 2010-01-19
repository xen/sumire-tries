#ifndef SUMIRE_OBJECT_READER_H
#define SUMIRE_OBJECT_READER_H

#include "int-types.h"

#include <cassert>
#include <iostream>

namespace sumire {

class ObjectReader
{
public:
	explicit ObjectReader(std::istream *input) : input_(input) {}

	template <typename OBJECT_TYPE>
	bool read(OBJECT_TYPE *obj, UInt32 num_objs = 1)
	{
		assert(obj != NULL || num_objs == 0);

		char *obj_ptr = reinterpret_cast<char *>(obj);
		if (!input_->read(obj_ptr, sizeof(OBJECT_TYPE) * num_objs))
			return false;
		return true;
	}

private:
	std::istream *input_;

	// Disallows copies.
	ObjectReader(const ObjectReader &);
	ObjectReader &operator=(const ObjectReader &);
};

}  // namespace sumire

#endif  // SUMIRE_OBJECT_READER_H
