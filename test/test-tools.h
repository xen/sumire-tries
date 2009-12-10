#ifndef TEST_TOOLS_H
#define TEST_TOOLS_H

#include <sumire/int-types.h>

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#define FIND_ALL_KEYS(trie, keys) \
	for (test::Tools::KeysIterator it = (keys).begin(); \
		it != (keys).end(); ++it) \
	{ \
		sumire::UInt32 value; \
		assert((trie).find(it->first.c_str(), &value)); \
		assert(value == it->second); \
		assert((trie).find(it->first.c_str(), it->first.length(), &value)); \
		assert(value == it->second); \
	}

namespace test {

class Tools
{
public:
	enum { NUM_KEYS = 1 << 16 };
	enum { KEY_LENGTH = 8 };
	enum { MAX_VALUE = 100 };

	typedef std::map<std::string, sumire::UInt32> KeysMap;
	typedef KeysMap::const_iterator KeysIterator;

	static void make_keys(KeysMap *keys)
	{
		std::srand(std::time(NULL));
		char key[KEY_LENGTH + 1] = "";
		while (keys->size() < NUM_KEYS)
		{
			for (int i = 0; i < KEY_LENGTH; ++i)
				key[i] = 'A' + (std::rand() % 26);
			keys->insert(std::make_pair(key, std::rand() % MAX_VALUE));
		}
	}

	template <typename TRIE_TYPE>
	static void build_trie(const KeysMap &keys, TRIE_TYPE *trie)
	{
		sumire::TrieBuilder builder;
		for (KeysIterator it = keys.begin(); it != keys.end(); ++it)
			assert(builder.insert(it->first.c_str(), it->second));
		assert(builder.finish() == true);

		FIND_ALL_KEYS(builder.virtual_trie(), keys);

		assert(trie->build(builder.virtual_trie()) == true);
		assert(trie->num_keys() == keys.size());

		FIND_ALL_KEYS(*trie, keys)

		assert(trie->build(*trie) == true);

		FIND_ALL_KEYS(*trie, keys)
	}

	template <typename TRIE_TYPE>
	static void test_reload(const TRIE_TYPE &trie, const KeysMap &keys)
	{
		std::stringstream stream;
		assert(trie.write(&stream) == true);

		TRIE_TYPE reloaded_trie;
		assert(reloaded_trie.read(&stream) == true);

		FIND_ALL_KEYS(reloaded_trie, keys)
	}

	template <typename TRIE_TYPE>
	static void test_map(const TRIE_TYPE &trie, const KeysMap &keys)
	{
		std::stringstream stream;
		assert(trie.write(&stream) == true);

		std::string written_trie = stream.str();

		TRIE_TYPE mapped_trie;
		mapped_trie.map(&written_trie[0]);

		FIND_ALL_KEYS(mapped_trie, keys)
	}

	template <typename TRIE_TYPE>
	static void test_clear(TRIE_TYPE *trie)
	{
		trie->clear();

		assert(trie->num_units() == 0);
		assert(trie->num_nodes() == 0);
		assert(trie->num_keys() == 0);
		assert(trie->size() == 0);
	}
};

}  // namespace sumire

#endif  // SUMIRE_TEST_TOOLS_H
