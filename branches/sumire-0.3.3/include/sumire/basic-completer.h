#ifndef SUMIRE_BASIC_COMPLETER_H
#define SUMIRE_BASIC_COMPLETER_H

#include "completer-base.h"

#include <vector>

namespace sumire {

class BasicCompleter : public CompleterBase
{
public:
	BasicCompleter() : trie_(NULL), key_(), index_stack_(), value_(0) {}
	~BasicCompleter() { clear(); }

	bool start(const TrieBase &trie, UInt32 index);
	bool next();

	const char *key() const;
	UInt32 length() const { return key_.size() - 1; }
	UInt32 value() const { return value_; }

	void clear();

private:
	const TrieBase *trie_;
	std::vector<UInt8> key_;
	std::vector<UInt32> index_stack_;
	UInt32 value_;

	// Disallows copies.
	BasicCompleter(const BasicCompleter &);
	BasicCompleter &operator=(const BasicCompleter &);

	bool find_next();
	bool find_value();

	void push_index(UInt32 index);
	void pop_index();
};

}  // namespace sumire

#include "basic-completer-in.h"

#endif  // SUMIRE_BASIC_COMPLETER_H
