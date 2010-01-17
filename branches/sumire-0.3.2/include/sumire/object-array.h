#ifndef SUMIRE_OBJECT_ARRAY_H
#define SUMIRE_OBJECT_ARRAY_H

#include "int-types.h"

#include <iostream>

namespace sumire {

template <typename OBJECT_TYPE>
class ObjectArray
{
public:
	typedef OBJECT_TYPE Object;

	ObjectArray() : objs_(NULL), num_objs_(0), needs_delete_(true) {}
	~ObjectArray() { clear(); }

	// This function discards old objects.
	void resize(UInt32 num_objs);

	const Object *begin() const { return objs_; }
	const Object *end() const { return objs_ + num_objs_; }
	Object *begin() { return objs_; }
	Object *end() { return objs_ + num_objs_; }

	const Object &front() const;
	const Object &back() const;
	Object &front();
	Object &back();

	const Object &operator[](UInt32 index) const;
	Object &operator[](UInt32 index);

	UInt32 num_objs() const { return num_objs_; }
	UInt32 size() const { return sizeof(Object) * num_objs_; }

	void clear();
	void *map(void *addr);
	void *map(void *addr, UInt32 num_objs);
	bool read(std::istream *input);
	bool write(std::ostream *output) const;

	void swap(ObjectArray *target);

private:
	Object *objs_;
	UInt32 num_objs_;
	bool needs_delete_;

	// Disallows copies.
	ObjectArray(const ObjectArray &);
	ObjectArray &operator=(const ObjectArray &);
};

}  // namespace sumire

#include "object-array-in.h"

#endif  // SUMIRE_OBJECT_ARRAY_H
