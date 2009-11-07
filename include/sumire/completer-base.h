#ifndef SUMIRE_COMPLETER_BASE_H
#define SUMIRE_COMPLETER_BASE_H

#include "int-types.h"

namespace sumire {

class CompleterBase
{
public:
	CompleterBase() {}
	virtual ~CompleterBase() {}

	virtual bool start(const TrieBase &trie, UInt32 index) = 0;
	virtual bool next() = 0;

	virtual const char *key() const = 0;
	virtual UInt32 length() const = 0;
	virtual UInt32 value() const = 0;

	virtual void clear() = 0;

private:
	// Disallows copies.
	CompleterBase(const CompleterBase &);
	CompleterBase &operator=(const CompleterBase &);
};

}  // namespace sumire

#endif  // SUMIRE__COMPLETER_BASE_H
