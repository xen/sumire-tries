# Outline #

This project provides a variety of trie implementations as C++ headers.

  * Array-based tries
    * BasicTrie: Sequential search trie
    * TernaryTrie: Binary search trie
    * DaTrie: Random access trie
  * Succinct tries
    * SuccinctTrie: Level-order binarized trie
    * LoudsTrie: Level-order unary degree sequence (LOUDS) trie
    * LoudsPlusTrie: LOUDS++ trie

# Documentation #

Refer to wiki pages.

  * ClassList
    * TrieBuilder
    * TrieBase
    * CompleterBase

# Examples #

The following example builds a trie for a couple of keys "foo" and "var".

```
#include <sumire/trie-builder.h>

sumire::TrieBuilder builder;

// Keys must be inserted in sorted order.
builder.insert("foo");
builder.insert("var");

// After inserting all keys, please invoke finish().
builder.finish();
```

After building a trie, you can convert it into any kind of trie.
The following example builds a double-array trie from **sumire::TrieBuilder**.
And other tries also have the same member function **build()** that is defined
as a common interface of tries, see **sumire::TrieBase**.

```
#include <sumire/da-trie.h>

sumire::DaTrie trie;

// Build a double-array trie from a trie.
trie.build(builder.virtual_trie());
```

Then, you can use that trie as a dictionary.

```
sumire::UInt32 value;
if (trie.find("foo", &value))
  std::cout << "foo: " << value << std::endl;
else
  std::cout << "foo: not found" << std::endl;
```