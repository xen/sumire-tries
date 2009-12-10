#include <sumire/object-pool.h>

#include <cassert>
#include <sstream>

#define CHECK_VALUES(pool) \
	for (sumire::UInt32 i = 0; i < POOL_SIZE; ++i) \
		assert((pool)[i] == i);

namespace {

typedef sumire::ObjectPool<sumire::UInt32> PoolType;

const sumire::UInt32 POOL_SIZE = 1 << 16;

void make_pool(PoolType *pool)
{
	assert(pool->block_size() == sizeof(PoolType::Object)
		* pool->num_objs_per_block());

	for (sumire::UInt32 i = 0; i < POOL_SIZE; ++i)
	{
		assert(pool->alloc() == i);

		(*pool)[i] = i;

		assert(pool->front() == 0);
		assert(pool->back() == i);
		assert((*pool)[i] == i);

		assert(pool->num_objs() == i + 1);
		assert(pool->num_blocks() == (pool->num_objs()
			+ pool->num_objs_per_block() - 1) / pool->num_objs_per_block());
	}

	CHECK_VALUES(*pool)
}

void test_io(const PoolType &pool)
{
	std::stringstream stream;

	assert(pool.write(&stream) == true);

	PoolType reloaded_pool;
	assert(reloaded_pool.read(&stream) == true);

	CHECK_VALUES(reloaded_pool)

	std::string written_pool = stream.str();

	PoolType mapped_pool;
	mapped_pool.map(&written_pool[0]);

	CHECK_VALUES(mapped_pool)
}

void clear_pool(PoolType *pool)
{
	pool->clear();

	assert(pool->num_objs() == 0);
	assert(pool->num_blocks() == 0);
	assert(pool->size() == 0);
}

}  // namespace

int main()
{
	PoolType pool;

	make_pool(&pool);
	test_io(pool);
	clear_pool(&pool);

	return 0;
}
