#include <sumire/trie-builder.h>
#include <sumire/basic-trie.h>
#include <sumire/ternary-trie.h>
#include <sumire/da-trie.h>
#include <sumire/succinct-trie.h>
#include <sumire/louds-plus-trie.h>
#include <sumire/louds-trie.h>
#include <sumire/basic-completer.h>

#include "test-tools.h"

namespace {

template <typename TRIE_TYPE>
void test_completer(const TRIE_TYPE &trie, const test::Tools::KeysMap &keys)
{
	sumire::BasicCompleter completer;
	assert(completer.start(trie, trie.root()) == true);

	test::Tools::KeysIterator key_it = keys.begin();
	while (completer.next())
	{
		assert(key_it != keys.end());
		test::Tools::KeysIterator result_it = keys.find(completer.key());
		assert(result_it == key_it);
		assert(result_it->second == completer.value());
		++key_it;
	}
	assert(key_it == keys.end());
}

template <typename TRIE_TYPE>
void test_trie(const sumire::TrieBase &basic_trie,
	const test::Tools::KeysMap &keys)
{
	TRIE_TYPE trie;
	trie.build(basic_trie);

	FIND_ALL_KEYS(trie, keys);

	test_completer(trie, keys);
}

}  // namespace

int main()
{
	test::Tools::KeysMap keys;
	test::Tools::make_keys(&keys);

	sumire::BasicTrie trie;
	test::Tools::build_trie(keys, &trie);

	test_trie<sumire::BasicTrie>(trie, keys);
	test_trie<sumire::TernaryTrie>(trie, keys);
	test_trie<sumire::DaTrie>(trie, keys);
	test_trie<sumire::SuccinctTrie<> >(trie, keys);
	test_trie<sumire::LoudsTrie<> >(trie, keys);
	test_trie<sumire::LoudsPlusTrie<> >(trie, keys);

	return 0;
}
