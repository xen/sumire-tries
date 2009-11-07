#include <sumire/basic-trie.h>
#include <sumire/succinct-trie.h>
#include <sumire/trie-builder.h>

#include "test-tools.h"

int main()
{
	std::vector<std::string> keys;
	test::Tools::read_keys("test-keys.txt", &keys);

	sumire::SuccinctTrie<> trie;
	test::Tools::build_trie(keys, &trie);

	test::Tools::test_reload(trie, keys);
	test::Tools::test_map(trie, keys);
	test::Tools::test_clear(&trie);

	return 0;
}
