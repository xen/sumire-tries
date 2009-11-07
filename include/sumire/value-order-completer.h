#ifndef SUMIRE_VALUE_ORDER_COMPLETER_H
#define SUMIRE_VALUE_ORDER_COMPLETER_H

#include "completer-base.h"
#include "value-order-completer-candidate.h"
#include "value-order-completer-node.h"

#include <vector>

namespace sumire {

class ValueOrderCompleter : public CompleterBase
{
public:
	ValueOrderCompleter() : trie_(NULL), key_(), value_(0),
		nodes_(), node_queue_(), candidate_queue_() {}
	~ValueOrderCompleter() { clear(); }

	bool start(const TrieBase &trie, UInt32 index);
	bool next();

	const char *key() const;
	UInt32 length() const { return key_.size() - 1; }
	UInt32 value() const { return value_; }

	void clear();

private:
	const TrieBase *trie_;
	std::vector<char> key_;
	UInt32 value_;
	std::vector<ValueOrderCompleterNode> nodes_;
	std::vector<UInt32> node_queue_;
	std::vector<ValueOrderCompleterCandidate> candidate_queue_;

	// Disallows copies.
	ValueOrderCompleter(const ValueOrderCompleter &);
	ValueOrderCompleter &operator=(const ValueOrderCompleter &);

	UInt32 find_sibling(UInt32 node_index);
	UInt32 find_leaf(UInt32 node_index);
	UInt32 add_node(UInt32 trie_index, UInt32 parent);

	void enqueue_node(UInt32 node_index);
	void enqueue_candidate(UInt32 node_index, UInt32 value);
	void dequeue_candidate();
	const ValueOrderCompleterCandidate &top_candidate();
};

}  // namespace sumire

#include "value-order-completer-in.h"

#endif  // SUMIRE_VALUE_ORDER_COMPLETER_H
