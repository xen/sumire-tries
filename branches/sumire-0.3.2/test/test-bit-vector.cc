#include <sumire/bit-vector.h>

#include <cassert>
#include <sstream>

#define CHECK_VALUES(bv) \
	for (sumire::UInt32 i = 0; i < TEST_SIZE; ++i) \
		assert((bv)[i] == to_flag(i));

namespace {

const sumire::UInt32 TEST_SIZE = 1 << 16;

bool to_flag(sumire::UInt32 value) { return (value & 1) ? true : false; }

void make_bv(sumire::BitVector *bv)
{
	for (sumire::UInt32 i = 0; i < TEST_SIZE; ++i)
	{
		bv->add(to_flag(i));

		assert((*bv)[i] == to_flag(i));

		assert(bv->num_bits() == i + 1);
	}

	CHECK_VALUES(*bv)
}

void test_io(const sumire::BitVector &bv)
{
	std::stringstream stream;

	assert(bv.write(&stream) == true);

	sumire::BitVector reloaded_bv;
	assert(reloaded_bv.read(&stream) == true);

	CHECK_VALUES(reloaded_bv)

	std::string written_bv = stream.str();

	sumire::BitVector mapped_bv;
	mapped_bv.map(&written_bv[0]);

	CHECK_VALUES(mapped_bv)
}

void clear_bv(sumire::BitVector *bv)
{
	bv->clear();

	assert(bv->num_units() == 0);
	assert(bv->num_bits() == 0);
	assert(bv->size() == 0);
	assert(bv->capacity() == 0);
}

}  // namespace

int main()
{
	sumire::BitVector bv;

	make_bv(&bv);
	test_io(bv);
	clear_bv(&bv);

	return 0;
}
