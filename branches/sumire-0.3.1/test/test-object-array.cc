#include <sumire/object-array.h>

#include <cassert>
#include <sstream>

#define CHECK_VALUES(array) \
	for (sumire::UInt32 i = 0; i < ARRAY_SIZE; ++i) \
	{ \
		assert((array)[i] == i); \
		assert(*((array).begin() + i) == i); \
		assert(*((array).end() + i - (array).num_objs()) == i); \
	}

namespace {

typedef sumire::ObjectArray<sumire::UInt32> ArrayType;

const sumire::UInt32 ARRAY_SIZE = 1 << 16;

void make_array(ArrayType *array)
{
	array->resize(ARRAY_SIZE);
	for (sumire::UInt32 i = 0; i < ARRAY_SIZE; ++i)
	{
		(*array)[i] = i;
		assert((*array)[i] == i);
	}

	assert(array->num_objs() == ARRAY_SIZE);
	assert(array->size() == sizeof(ArrayType::Object) * ARRAY_SIZE);

	CHECK_VALUES(*array)
}

void test_io(const ArrayType &array)
{
	std::stringstream stream;

	assert(array.write(&stream) == true);

	ArrayType reloaded_array;
	assert(reloaded_array.read(&stream) == true);

	CHECK_VALUES(reloaded_array)

	std::string written_array = stream.str();

	ArrayType mapped_array;
	mapped_array.map(&written_array[0]);

	CHECK_VALUES(mapped_array)
}

void clear_array(ArrayType *array)
{
	array->clear();

	assert(array->num_objs() == 0);
	assert(array->size() == 0);
}

}  // namespace

int main()
{
	ArrayType array;

	make_array(&array);
	test_io(array);
	clear_array(&array);

	return 0;
}
