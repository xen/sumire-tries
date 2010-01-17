#ifndef SUMIRE_VALUE_ORDER_COMPLETER_NODE_H
#define SUMIRE_VALUE_ORDER_COMPLETER_NODE_H

#include "int-types.h"

namespace sumire {

class ValueOrderCompleterNode
{
public:
	ValueOrderCompleterNode() : trie_index_(0), parent_(0),
		label_('\0'), is_queued_(false) {}

	void set_trie_index(UInt32 index) { trie_index_ = index; }
	void set_parent(UInt32 index) { parent_ = index; }
	void set_label(UInt8 label) { label_ = label; }
	void set_is_queued() { is_queued_ = true; }

	UInt32 trie_index() const { return trie_index_; }
	UInt32 parent() const { return parent_; }
	UInt8 label() const { return label_; }
	bool is_queued() const { return is_queued_; }

private:
	UInt32 trie_index_;
	UInt32 parent_;
	UInt8 label_;
	bool is_queued_;

	// Copyable.
};

}  // namespace sumire

#endif  // SUMIRE_VALUE_ORDER_COMPLETER_NODE_H
