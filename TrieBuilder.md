# Introduction #

sumire::TrieBuilder is a class to build a trie.

In sumire-tries, keys are associated with 31-bit unsigned integers
called values and the values are used as IDs, weights, etc.

Note that TrieBuilder builds a trie from a **sorted** lexicon.
This means that keys must be inserted in sorted order,
e.g. "apple" cannot be inserted after "orange".
Therefore, if there is an unsorted lexicon given as std::vector<std::string>,
the lexicon needs to be sorted by std::sort() or other ways.
In some cases, std::map<std::string, int> could be an alternative of
std::vector<std::string>.

TrieBuilder can handle a large lexicon which consists of millions of keys.
However, for such a large lexicon, std::vector<std::string> and
std::map<std:string, int> are time and memory consuming.
Reading and inserting keys one by one may be a better way.

# Examples #

The following example builds a trie from an unsorted lexicon given through
the standard input stream.

```
#include <sumire/trie-builder.h>
#include <sumire/basic-trie.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main()
{
  // Reads a lexicon from the standard input stream.
  std::vector<std::string> keys;
  std::string line;
  while (std::getline(std::cin, line))
    keys.push_back(line);

  // Sorts a lexicon and removes repeated keys.
  std::sort(keys.begin(), keys.end());
  keys.erase(std::unique(keys.begin(), keys.end()), keys.end());

  // Inserts keys associated with IDs.
  sumire::TrieBuilder trie_builder;
  for (std::size_t key_id = 0; key_id < keys.size(); ++key_id)
  {
    if (!trie_builder.insert(keys[key_id].c_str(),
        static_cast<sumire::UInt32>(key_id)))
    {
      std::cerr << "error: failed to insert a key: "
                << keys[key_id] << std::endl;
      return 1;
    }
  }

  // Finishes inserting keys.
  if (!trie_builder.finish())
  {
    std::cerr << "error: failed to finish building a trie" << std::endl;
    return 2;
  }

  // Builds another kind of trie from a virtual trie of TrieBuilder.
  sumire::BasicTrie trie;
  if (!trie.build(trie_builder.virtual_trie()))
  {
    std::cerr << "error: failed to convert a trie" << std::endl;
    return 3;
  }

  return 0;
}
```

The next example reads keys from a sorted lexicon one by one.
For simplicity, error messages are omitted.

```
#include <sumire/trie-builder.h>

#include <iostream>
#include <string>

int main()
{
  sumire::UInt32 key_id = 0;
  sumire::TrieBuilder trie_builder;

  std::string key;
  while (std::getline(std::cin, key))
  {
    // Note that if a lexicon contains a repeated key,
    // the latter one will be used.
    if (!trie_builder.insert(key.c_str(), key_id++))
      return 1;
  }

  if (!trie_builder.finish())
    return 2;

  return 0;
}
```

# Details #

```
#include <sumire/trie-builder.h>
namespace sumire

class TrieBuilder
{
public:
  TrieBuilder();

  const VirtualTrie &virtual_trie();

  bool insert(const char *key, UInt32 value = 0);
  bool insert(const char *key, UInt32 length, UInt32 value);
  bool finish();

  bool initialized() const;
  bool finished() const;

  UInt32 num_units() const;
  UInt32 num_nodes() const;
  UInt32 num_keys() const;
  UInt32 size() const;

  void clear();

  void swap(TrieBuilder *target);
};
```


---


```
const VirtualTrie &TrieBuilder::virtual_trie()
```

This function returns a virtual trie which partially implements
sumire::TrieBase.

Note that this function must be called after finish(),
and the virtual trie is destroyed by clear() or ~TrieBuilder().

A virtual trie is used as a source of a variety of tries by giving it
to build() of sumire::TrieBase.


---


```
bool TrieBuilder::insert(const char *key, UInt32 value = 0);
bool TrieBuilder::insert(const char *key, UInt32 length, UInt32 value);
```

This function inserts a key value pair and returns whether the insertion has
succeeded (true) or not (false).

If the number of arguments is less than 3, the key is handled as a string
terminated by zero.

An insertion fails if the key length == 0, the value >= 2^31,
the inserting key < the last inserted key,
or finish() has been already called.


---


```
bool TrieBuilder::finish();
```

This function completes a trie and returns false iff already completed.