# Introduction #

sumire::CompleterBase is the base class of completers.

In sumire-tries, key completion is to find keys starting with a specified
sequence, e.g. a completer finds "apple" and "application" when "app" is
given as the query.

CompleterBase defines a common interface for interactive key completion.
The "interactive" means that a completer finds completed keys one by one,
i.e. a system can terminate key completion any time.
The common interface provides start() to initialize key completion and
next() to get the first or next completed key.

A completer makes a traversal on a trie and returns completed keys,
so the order of completed keys depends on how the traversal behaves.
For example, sumire::BasicCompleter makes a depth-first (preorder)
traversal, and thus the order of completed keys depends on the order
of keys in the trie.
And usually, sumire::BasicCompleter returns completed keys in sorted
order because keys are inserted in sorted order.

However, if the keys are specially ordered by using build() of
sumire::BasicTrie, the order of completed keys changes.
Also, there is a special combination that uses sumire::ValueOrderCompleter
and a trie which is built with sumire::BasicTrie::MAX\_VALUE\_ORDER.
This combination returns completed keys in descending order of the value,
and this value order key completion is called weighted key completion.

See also the documentation for derived classes.

  * Derived classes
    * BasicCompleter
    * ValueOrderCompleter

# Examples #

The following example gives a function to find keys having a specified prefix.
This function writes the completed keys to the standard output stream.
The number of keys to be written is limited by the third argument.

```
#include <sumire/trie-base.h>
#include <sumire/basic-completer.h>

#include <iostream>

bool Complete(const sumire::TrieBase &trie, const char *prefix, int num_keys)
{
  sumire::UInt32 index = trie.root();
  if (!trie.follow(index, prefix, &index))
    return false;

  sumire::BasicCompleter completer;
  if (!completer.start(trie, index))
    return false;

  for (int i = 0; i < num_keys; ++i)
  {
    if (!completer.next())
      break;

    // Shows the completed key and its value.
    std::cout << prefix << completer.key()
              << ": " << completer.value() << std::endl;
  }

  return true;
}
```

# Details #

```
class CompleterBase
{
public:
  CompleterBase();
  virtual ~CompleterBase();

  virtual bool start(const TrieBase &trie, UInt32 index) = 0;
  virtual bool next() = 0;

  virtual const char *key() const = 0;
  virtual UInt32 length() const = 0;
  virtual UInt32 value() const = 0;

  virtual void clear() = 0;
};
```


---


```
virtual bool CompleterBase::start(const TrieBase &trie, UInt32 index) = 0
```

This function initializes key completion and returns if the initialization
has succeeded (true) or not (false).

An initialization fails iff the given trie is empty.

After this function returns true, next() progresses key completion.


---


```
virtual bool CompleterBase::next() = 0
```

This function finds the first or next key and returns true if exists,
otherwise it returns false.

Note that this function must be called after start() returns true,
and when this function has returned true, the key and its value are
available by using key(), length(), and value().


---


```
virtual const char *CompleterBase::key() const = 0
```

This function returns the completed part of the current key,
e.g. if the query is "app" and the current key is "apple",
this function returns only "le".

The returned string is terminated by zero.

Note that key(), length(), and value() must be called after next() returns true.
Also, a pointer returned by key() will be invalid after the next call of next().


---


```
virtual UInt32 CompleterBase::length() const = 0
```

This function returns the length of the completed part of the current key.


---


```
virtual UInt32 CompleterBase::value() const = 0
```

This function returns the value associated with the current key.