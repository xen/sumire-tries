#include <sumire/basic-trie.h>
#include <sumire/ternary-trie.h>
#include <sumire/trie-builder.h>

#include "test-tools.h"

int main()
{
	test::Tools::KeysMap keys;
	test::Tools::make_keys(&keys);

	sumire::TernaryTrie trie;
	test::Tools::build_trie(keys, &trie);

	test::Tools::test_reload(trie, keys);
	test::Tools::test_map(trie, keys);
	test::Tools::test_clear(&trie);

	return 0;
}
