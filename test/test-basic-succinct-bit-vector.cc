#include <sumire/basic-succinct-bit-vector.h>
#include <sumire/hybrid-succinct-bit-vector.h>
#include <sumire/simplified-succinct-bit-vector.h>

#include <cassert>
#include <sstream>

#define CHECK_VALUES(sbv) \
	for (sumire::UInt32 i = 0; i < TEST_SIZE; ++i) \
	{ \
		assert((sbv)[i] == to_flag(i)); \
		assert((sbv).rank_1(i) == (i + 1) / 2); \
		assert((sbv).rank_0(i) == (i / 2) + 1); \
	} \
	for (sumire::UInt32 i = 0; i < TEST_SIZE / 2; ++i) \
	{ \
		assert((sbv).select_1(i + 1) == (i * 2) + 1); \
		assert((sbv).select_0(i + 1) == (i * 2)); \
	}

namespace {

const sumire::UInt32 TEST_SIZE = (1 << 16) + 100;

bool to_flag(sumire::UInt32 value) { return (value & 1) ? true : false; }

template <typename SUCCINCT_BIT_VECTOR_TYPE>
void make_sbv(SUCCINCT_BIT_VECTOR_TYPE *sbv)
{
	sumire::BitVector bv;
	for (sumire::UInt32 i = 0; i < TEST_SIZE; ++i)
		bv.add(to_flag(i));
	sbv->build(bv);

	assert(sbv->num_bits() == TEST_SIZE);

	CHECK_VALUES(*sbv)
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
void test_io(const SUCCINCT_BIT_VECTOR_TYPE &sbv)
{
	std::stringstream stream;

	assert(sbv.write(&stream) == true);

	SUCCINCT_BIT_VECTOR_TYPE reloaded_sbv;
	assert(reloaded_sbv.read(&stream) == true);

	CHECK_VALUES(reloaded_sbv)

	std::string written_sbv = stream.str();

	SUCCINCT_BIT_VECTOR_TYPE mapped_sbv;
	mapped_sbv.map(&written_sbv[0]);

	CHECK_VALUES(mapped_sbv)
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
void clear_sbv(SUCCINCT_BIT_VECTOR_TYPE *sbv)
{
	sbv->clear();

	assert(sbv->num_bits() == 0);
	assert(sbv->num_ones() == 0);
	assert(sbv->num_zeros() == 0);
	assert(sbv->size() == 0);
}

template <typename SUCCINCT_BIT_VECTOR_TYPE>
void test_sbv(const char *type_name)
{
	std::cerr << type_name << std::endl;

	SUCCINCT_BIT_VECTOR_TYPE sbv;

	make_sbv(&sbv);
	test_io(sbv);
	clear_sbv(&sbv);
}

}  // namespace

int main()
{
#define TEST_SBV(type) test_sbv<type>(#type);

	TEST_SBV(sumire::BasicSuccinctBitVector);
	TEST_SBV(sumire::HybridSuccinctBitVector);
	TEST_SBV(sumire::SimplifiedSuccinctBitVector);

#undef TEST_SBV

	return 0;
}
