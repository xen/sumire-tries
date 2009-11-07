#include <sumire/trie-builder.h>
#include <sumire/basic-trie.h>
#include <sumire/ternary-trie.h>
#include <sumire/da-trie.h>
#include <sumire/simplified-succinct-bit-vector.h>
#include <sumire/hybrid-succinct-bit-vector.h>
#include <sumire/succinct-trie.h>
#include <sumire/louds-trie.h>
#include <sumire/louds-plus-trie.h>
#include <sumire/basic-completer.h>
#include <sumire/value-order-completer.h>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

namespace {

class TimeWatch
{
public:
	explicit TimeWatch(const char *msg = NULL)
		: msg_(msg), cl_(std::clock()) {}
	~TimeWatch()
	{
		double elapsed = 1.0 * (std::clock() - cl_) / CLOCKS_PER_SEC;
		if (msg_ != NULL)
			std::cout << msg_ << ' ';
		std::cout << "time (s): " << elapsed << std::endl;
	}

private:
	const char *msg_;
	std::clock_t cl_;

	// Disallows copies.
	TimeWatch(const TimeWatch &);
	TimeWatch &operator=(const TimeWatch &);
};

class RandomNumberGenerator
{
public:
	std::ptrdiff_t operator()(std::ptrdiff_t limit) const
	{
		return ((std::rand() * (static_cast<std::ptrdiff_t>(RAND_MAX) + 1))
			+ std::rand()) % limit;
	}
};

void read_keys(std::istream *input, std::vector<std::string> *keys)
{
	std::size_t num_empty_lines = 0;

	std::cerr << "reading keys...";

	std::string line;
	while (std::getline(std::cin, line))
	{
		if (line.empty())
			++num_empty_lines;
		else
			keys->push_back(line);
	}

	std::cerr << " done" << std::endl;

	if (num_empty_lines > 0)
		std::cout << "#skipped empty lines: " << num_empty_lines << std::endl;

	std::cout << "#lines: " << keys->size() << std::endl;
}

void sort_keys(std::vector<std::string> *keys)
{
	std::cerr << "sorting keys...";

	std::sort(keys->begin(), keys->end());
	keys->erase(std::unique(keys->begin(), keys->end()), keys->end());

	std::cerr << " done" << std::endl;

	std::cout << "#distinct lines: " << keys->size() << std::endl;
}

void randomize_keys(const std::vector<std::string> &keys,
	std::vector<std::string> *random_keys)
{
	*random_keys = keys;

	RandomNumberGenerator gen;
	std::random_shuffle(random_keys->begin(), random_keys->end(), gen);
}

bool build_trie(const std::vector<std::string> &keys,
	sumire::TrieBuilder *builder)
{
	TimeWatch watch("build");

	RandomNumberGenerator gen;
	for (std::size_t i = 0; i < keys.size(); ++i)
	{
		if (!builder->insert(keys[i].c_str(),
			(gen(sumire::UInt32Limits::max() >> 1) + 1)))
		{
			std::cerr << "error: failed to insert key: "
				<< keys[i] << std::endl;
			return false;
		}
	}

	return builder->finish();
}

bool build_value_order_trie(const sumire::TrieBase &src_trie,
	sumire::BasicTrie *value_order_trie)
{
	TimeWatch watch("build");

	return value_order_trie->build(
		src_trie, sumire::BasicTrie::MAX_VALUE_ORDER);
}

bool build_breadth_order_trie(const sumire::TrieBase &src_trie,
	sumire::BasicTrie *breadth_order_trie)
{
	TimeWatch watch("build");

	return breadth_order_trie->build(
		src_trie, sumire::BasicTrie::BREADTH_ORDER);
}

template <typename TRIE_TYPE>
bool build_trie(const sumire::TrieBase &src_trie, TRIE_TYPE *dest_trie)
{
	TimeWatch watch("build");

	return dest_trie->build(src_trie);
}

template <typename TRIE_TYPE>
bool find_keys(const TRIE_TYPE &trie, const std::vector<std::string> &keys)
{
	TimeWatch watch("find");

	for (std::size_t i = 0; i < keys.size(); ++i)
	{
		if (!trie.find(keys[i].c_str()))
		{
			std::cerr << "error: failed to find key: "
				<< keys[i] << std::endl;
			return false;
		}
	}
	return true;
}

template <typename COMPLETER_TYPE, typename TRIE_TYPE>
bool complete_keys(const TRIE_TYPE &trie)
{
	TimeWatch watch("complete");

	COMPLETER_TYPE completer;
	if (!completer.start(trie, trie.root()))
	{
		std::cerr << "error: failed to start completion" << std::endl;
		return false;
	}

	while (completer.next())
	{
		if (completer.length() == 0)
		{
			std::cerr << "error: failed to complete keys" << std::endl;
			return false;
		}
	}
	return true;
}

template <typename TRIE_TYPE>
bool trie_benchmark(const char *trie_type_name,
	const sumire::TrieBase &src_trie,
	const std::vector<std::string> &sorted_keys,
	const std::vector<std::string> &random_keys)
{
	std::cout << "--> " << trie_type_name << " <--" << std::endl;

	TRIE_TYPE trie;
	if (!build_trie(src_trie, &trie))
		return false;

	std::cout << "size: " << trie.size() << std::endl;

	if (!find_keys(trie, sorted_keys))
		return false;

	if (!find_keys(trie, random_keys))
		return false;

	if (!complete_keys<sumire::BasicCompleter>(trie))
		return false;

	if (!complete_keys<sumire::ValueOrderCompleter>(trie))
		return false;

	return true;
}

void trie_benchmark(const sumire::TrieBase &src_trie,
	const std::vector<std::string> &sorted_keys,
	const std::vector<std::string> &random_keys)
{
#define TRIE_BENCHMARK(trie_type) \
	trie_benchmark<trie_type>(#trie_type, src_trie, sorted_keys, random_keys)

	TRIE_BENCHMARK(sumire::BasicTrie);
	TRIE_BENCHMARK(sumire::TernaryTrie);
	TRIE_BENCHMARK(sumire::DaTrie);
	TRIE_BENCHMARK(sumire::SuccinctTrie<sumire::BasicSuccinctBitVector>);
	TRIE_BENCHMARK(sumire::LoudsTrie<sumire::BasicSuccinctBitVector>);
	TRIE_BENCHMARK(sumire::LoudsPlusTrie<sumire::BasicSuccinctBitVector>);
	TRIE_BENCHMARK(sumire::SuccinctTrie<sumire::SimplifiedSuccinctBitVector>);
	TRIE_BENCHMARK(sumire::LoudsTrie<sumire::SimplifiedSuccinctBitVector>);
	TRIE_BENCHMARK(sumire::LoudsPlusTrie<sumire::SimplifiedSuccinctBitVector>);
	TRIE_BENCHMARK(sumire::SuccinctTrie<sumire::HybridSuccinctBitVector>);
	TRIE_BENCHMARK(sumire::LoudsTrie<sumire::HybridSuccinctBitVector>);
	TRIE_BENCHMARK(sumire::LoudsPlusTrie<sumire::HybridSuccinctBitVector>);

#undef TRIE_BENCHMARK
}

}  // namespace

int main()
{
	std::vector<std::string> keys;
	read_keys(&std::cin, &keys);
	sort_keys(&keys);
	if (keys.size() == 0)
	{
		std::cerr << "error: no keys" << std::endl;
		return 1;
	}

	std::vector<std::string> random_keys;
	randomize_keys(keys, &random_keys);

	std::cout << "\n==> benchmark for dictionary order tries <==" << std::endl;

	sumire::TrieBuilder builder;
	build_trie(keys, &builder);

	trie_benchmark(builder.virtual_trie(), keys, random_keys);

	{
		std::cout << "\n==> benchmark for breadth order tries <==" << std::endl;

		sumire::BasicTrie breadth_order_trie;
		build_breadth_order_trie(builder.virtual_trie(), &breadth_order_trie);

		trie_benchmark(breadth_order_trie, keys, random_keys);
	}

	{
		std::cout << "\n==> benchmark for value order tries <==" << std::endl;

		sumire::BasicTrie value_order_trie;
		build_value_order_trie(builder.virtual_trie(), &value_order_trie);

		trie_benchmark(value_order_trie, keys, random_keys);
	}

	return 0;
}
