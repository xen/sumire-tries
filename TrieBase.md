# Introduction #

sumire::TrieBase is the base class of a variety of tries.

TrieBase declares a common interface and defines useful functions.
The common interface provides functions to traverse a trie,
and those functions are used in completers.
Also, the common interface provides functions for I/O through std::iostream.
In addition, for the use of memory mapped files, a function is provided
for mapping specified memory to a trie.

Due to this base class, different kinds of tries can be easily tested.

See also the documentation for derived classes.

  * Derived classes
    * BasicTrie
    * TernaryTrie
    * DaTrie
    * SuccinctTrie<>
    * LoudsTrie<>
    * LoudsPlusTrie<>

# Examples #

The following example tests if keys and values are correctly registered or not.

```
#include <sumire/trie-base.h>

#include <string>
#include <map>

bool TestTrie(const sumire::TrieBase &trie, std::map<std::string, sumire::UInt32> &lexicon)
{
  for (std::map<std::string, sumire::UInt32>::const_iterator it = lexicon.begin();
       it != lexicon.end(); ++it)
  {
    sumire::UInt32 value;
    if (!trie.find(it->first.c_str(), &value))
    {
      std::cerr << "error: failed to find a key: "
                << it->first << std::endl;
      return false;
    }
    else if (value != it->second)
    {
      std::cerr << "error: found a wrong key value pair: "
                << it->first << ", " << it->second << std::endl;
      return false;
    }
  }

  return true;
}
```

The next example gives functions for file I/O.

```
#include <sumire/trie-base.h>

#include <fstream>

bool SaveTrie(const char *path, const sumire::TrieBase &trie)
{
  std::ofstream file(path, std::ios::binary);
  if (!file)
  {
    std::cerr << "error: failed to open a file: " << path << std::endl;
    return false;
  }

  if (!trie.write(&file))
  {
    std::cerr << "error: failed to write a trie to file: "
              << path << std::endl;
    return false;
  }

  return true;
}

bool LoadTrie(const char *path, sumire::TrieBase *trie)
{
  std::ifstream file(path, std::ios::binary);
  if (!file)
  {
    std::cerr << "error: failed to open a file: " << path << std::endl;
    return false;
  }

  if (!trie->read(&file))
  {
    std::cerr << "error: failed to read a trie from file: "
              << path << std::endl;
    return false;
  }

  return true;
}
```

# Details #

```
#include <sumire/trie-base.h>
namespace sumire

class TrieBase
{
public:
  static const UInt32 INVALID_VALUE = static_cast<UInt32>(-1);

  TrieBase();
  virtual ~TrieBase();

  virtual bool build(const TrieBase &trie) = 0;

  bool find(const char *key, UInt32 *value_ptr = NULL) const;
  bool find(const char *key, UInt32 length, UInt32 *value_ptr = NULL) const;

  bool follow(UInt32 index, const char *seq, UInt32 *index_ptr) const;
  bool follow(UInt32 index, const char *seq, UInt32 length, UInt32 *index_ptr) const;

  virtual UInt32 find_child(UInt32 index, UInt8 child_label) const = 0;

  virtual UInt32 child(UInt32 index) const = 0;
  virtual UInt32 sibling(UInt32 index) const = 0;
  virtual UInt8 label(UInt32 index) const = 0;

  virtual bool get_value(UInt32 index, UInt32 *value_ptr = NULL) const = 0;
  bool has_value(UInt32 index) const;
  UInt32 value(UInt32 index) const;

  UInt32 root() const;

  virtual UInt32 num_units() const = 0;
  virtual UInt32 num_nodes() const = 0;
  virtual UInt32 num_keys() const = 0;
  virtual UInt32 size() const = 0;

  virtual void clear() = 0;
  virtual void *map(void *addr) = 0;
  virtual bool read(std::istream *input) = 0;
  virtual bool write(std::ostream *output) const = 0;
};
```


---


```
virtual bool TrieBase::build(const TrieBase &trie) = 0
```

This function builds a trie from another trie and returns
if it has succeeded (true) or not (false).


---


```
bool TrieBase::find(const char *key, UInt32 *value_ptr = NULL) const
bool TrieBase::find(const char *key, UInt32 length, UInt32 *value_ptr = NULL) const
```

These functions check if the trie contains a given key or not
and returns true or false.
Also, the value of that key is assigned to value\_ptr if value\_ptr != NULL.


---


```
bool TrieBase::follow(UInt32 index, const char *seq, UInt32 *index_ptr) const
bool TrieBase::follow(UInt32 index, const char *seq, UInt32 length, UInt32 *index_ptr) const
```

These functions follow a path corresponding to a given sequence and returns
true if such a path exists, otherwise these functions return false.
Also, the end index of that path is assigned to index\_ptr if index\_ptr != NULL.


---


```
virtual UInt32 TrieBase::find_child(UInt32 index, UInt8 child_label) const = 0
```

This function searches child nodes for a node having a specified label
and returns its index if exists, otherwise this function returns 0.


---


```
virtual UInt32 TrieBase::child(UInt32 index) const = 0
```

This function returns the index of the first child if exists,
otherwise it returns 0.


---


```
virtual UInt32 TrieBase::sibling(UInt32 index) const = 0
```

This function returns the index of the next sibling if exists,
otherwise it returns 0.


---


```
virtual UInt8 TrieBase::label(UInt32 index) const = 0
```

This function returns the attached label.


---


```
virtual bool TrieBase::get_value(UInt32 index, UInt32 *value_ptr = NULL) const = 0
```

This function checks if a specified node is a terminal or not,
and returns true or false.
Also, the value of that terminal is assigned to value\_ptr if value\_ptr != NULL.


---


```
bool TrieBase::has_value(UInt32 index) const
```

This function is equal to get\_value(index).


---


```
UInt32 TrieBase::value(UInt32 index) const
```

This function calls get\_value() and returns a value if it returns true,
otherwise this function returns INVALID\_VALUE.


---


```
UInt32 TrieBase::root() const
```

This function returns the root index which is used as the start position
of traversing a trie.