#include <sumire/basic-trie.h>
#include <sumire/trie-builder.h>

#include "test-tools.h"

namespace {

void build_trie(const std::vector<std::string> &keys,
	sumire::BasicTrie *trie)
{
	sumire::TrieBuilder builder;
	for (sumire::UInt32 key_id = 0; key_id < keys.size(); ++key_id)
		assert(builder.insert(keys[key_id].c_str(), key_id));

	assert(builder.finish() == true);

	std::stringstream stream;
	assert(builder.virtual_trie().write(&stream) == true);

	assert(trie->read(&stream) == true);

	FIND_ALL_KEYS(*trie, keys)

	assert(trie->build(builder.virtual_trie()) == true);
	assert(trie->num_keys() == keys.size());

	FIND_ALL_KEYS(*trie, keys)
}

void convert_trie(const std::vector<std::string> &keys,
	const sumire::BasicTrie &src_trie)
{
	sumire::BasicTrie trie;
	assert(trie.build(src_trie, sumire::BasicTrie::LEVEL_ORDER) == true);

	assert(trie.num_units() == src_trie.num_units());
	assert(trie.num_keys() == keys.size());

	FIND_ALL_KEYS(trie, keys)

	assert(trie.build(src_trie, sumire::BasicTrie::BREADTH_ORDER) == true);

	assert(trie.num_units() == src_trie.num_units());
	assert(trie.num_keys() == keys.size());

	FIND_ALL_KEYS(trie, keys)

	assert(trie.build(src_trie, sumire::BasicTrie::MAX_VALUE_ORDER) == true);

	assert(trie.num_units() == src_trie.num_units());
	assert(trie.num_keys() == keys.size());

	FIND_ALL_KEYS(trie, keys)

	assert(trie.build(src_trie, sumire::BasicTrie::TOTAL_VALUE_ORDER) == true);

	assert(trie.num_units() == src_trie.num_units());
	assert(trie.num_keys() == keys.size());

	FIND_ALL_KEYS(trie, keys)
}

}  // namespace

int main()
{
	std::vector<std::string> keys;
	test::Tools::read_keys("test-keys.txt", &keys);

	sumire::BasicTrie trie;

	build_trie(keys, &trie);
	convert_trie(keys, trie);

	test::Tools::test_reload(trie, keys);
	test::Tools::test_map(trie, keys);
	test::Tools::test_clear(&trie);

	return 0;
}
