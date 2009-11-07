#ifndef TEST_TOOLS_H
#define TEST_TOOLS_H

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define FIND_ALL_KEYS(trie, keys) \
	for (sumire::UInt32 i = 0; i < (keys).size(); ++i) \
	{ \
		sumire::UInt32 value; \
		assert((trie).find((keys)[i].c_str(), &value)); \
		assert(value == i); \
		assert((trie).find((keys)[i].c_str(), (keys)[i].length(), &value)); \
		assert(value == i); \
	}

namespace test {

class Tools
{
public:
	static bool read_keys(const char *file_name,
		std::vector<std::string> *keys)
	{
		std::ifstream file(file_name, std::ios::binary);
		if (!file)
		{
			std::cerr << "error: failed to open file: "
				<< file_name << std::endl;
			return false;
		}

		std::string line;
		while (std::getline(file, line))
			keys->push_back(line);
		return true;
	}

	template <typename TRIE_TYPE>
	static void build_trie(const std::vector<std::string> &keys,
		TRIE_TYPE *trie)
	{
		sumire::TrieBuilder builder;
		for (sumire::UInt32 key_id = 0; key_id < keys.size(); ++key_id)
			assert(builder.insert(keys[key_id].c_str(), key_id));
		assert(builder.finish() == true);

		FIND_ALL_KEYS(builder.virtual_trie(), keys);

		assert(trie->build(builder.virtual_trie()) == true);
		assert(trie->num_keys() == keys.size());

		FIND_ALL_KEYS(*trie, keys)

		assert(trie->build(*trie) == true);

		FIND_ALL_KEYS(*trie, keys)
	}

	template <typename TRIE_TYPE>
	static void test_reload(const TRIE_TYPE &trie,
		const std::vector<std::string> &keys)
	{
		std::stringstream stream;
		assert(trie.write(&stream) == true);

		TRIE_TYPE reloaded_trie;
		assert(reloaded_trie.read(&stream) == true);

		FIND_ALL_KEYS(reloaded_trie, keys)
	}

	template <typename TRIE_TYPE>
	static void test_map(const TRIE_TYPE &trie,
		const std::vector<std::string> &keys)
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
