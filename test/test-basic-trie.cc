#include <sumire/basic-trie.h>
#include <sumire/trie-builder.h>

#include "test-tools.h"

namespace {

void build_trie(const test::Tools::KeysMap &keys, sumire::BasicTrie *trie)
{
	sumire::TrieBuilder builder;
	for (test::Tools::KeysIterator it = keys.begin(); it != keys.end(); ++it)
		assert(builder.insert(it->first.c_str(), it->second));

	assert(builder.finish() == true);

	std::stringstream stream;
	assert(builder.virtual_trie().write(&stream) == true);

	assert(trie->read(&stream) == true);

	FIND_ALL_KEYS(*trie, keys)

	assert(trie->build(builder.virtual_trie()) == true);
	assert(trie->num_keys() == keys.size());

	FIND_ALL_KEYS(*trie, keys)
}

void convert_trie(const test::Tools::KeysMap &keys,
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
	test::Tools::KeysMap keys;
	test::Tools::make_keys(&keys);

	sumire::BasicTrie trie;

	build_trie(keys, &trie);
	convert_trie(keys, trie);

	test::Tools::test_reload(trie, keys);
	test::Tools::test_map(trie, keys);
	test::Tools::test_clear(&trie);

	return 0;
}
