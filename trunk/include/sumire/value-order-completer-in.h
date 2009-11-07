#ifndef SUMIRE_VALUE_ORDER_COMPLETER_IN_H
#define SUMIRE_VALUE_ORDER_COMPLETER_IN_H

#include <algorithm>
#include <cassert>

namespace sumire {

inline bool ValueOrderCompleter::start(
	const TrieBase &trie, UInt32 trie_index)
{
	if (trie.num_units() == 0)
		return false;

	assert(trie_index < trie.num_units());

	clear();

	trie_ = &trie;
	enqueue_node(add_node(trie_index, 0));

	return true;
}

inline bool ValueOrderCompleter::next()
{
	assert(trie_ != NULL);

	for (UInt32 index = 0; index < node_queue_.size(); ++index)
	{
		UInt32 node_index = node_queue_[index];
		if (value_ != TrieBase::INVALID_VALUE)
		{
			node_index = find_sibling(node_index);
			if (node_index == 0)
				continue;
		}
		find_leaf(node_index);
	}
	node_queue_.clear();

	if (candidate_queue_.empty())
		return false;

	const ValueOrderCompleterCandidate &candidate = top_candidate();

	key_.clear();
	for (UInt32 node_index = candidate.index(); node_index != 0;
		node_index = nodes_[node_index].parent())
	{
		key_.push_back(nodes_[node_index].label());
		enqueue_node(node_index);
	}
	std::reverse(key_.begin(), key_.end());
	key_.push_back('\0');

	value_ = candidate.value();
	dequeue_candidate();

	return true;
}

inline const char *ValueOrderCompleter::key() const
{
	assert(key_.size() > 0);

	return &key_[0];
}

inline void ValueOrderCompleter::clear()
{
	trie_ = NULL;
	key_.clear();
	value_ = TrieBase::INVALID_VALUE;
	nodes_.clear();
	node_queue_.clear();
	candidate_queue_.clear();
}

inline UInt32 ValueOrderCompleter::find_sibling(UInt32 node_index)
{
	assert(node_index < nodes_.size());

	UInt32 trie_index = nodes_[node_index].trie_index();
	trie_index = trie_->sibling(trie_index);
	if (trie_index == 0)
		return 0;

	return add_node(trie_index, nodes_[node_index].parent());
}

inline UInt32 ValueOrderCompleter::find_leaf(UInt32 node_index)
{
	assert(node_index < nodes_.size());

	UInt32 trie_index = trie_->child(nodes_[node_index].trie_index());
	while (trie_index != 0)
	{
		node_index = add_node(trie_index, node_index);
		trie_index = trie_->child(trie_index);
	}
	return node_index;
}

inline UInt32 ValueOrderCompleter::add_node(UInt32 trie_index, UInt32 parent)
{
	assert(trie_index < trie_->num_units());
	assert((parent == 0 && nodes_.size() == 0) || parent < nodes_.size());

	UInt32 node_index = nodes_.size();
	nodes_.resize(node_index + 1);

	nodes_[node_index].set_trie_index(trie_index);
	nodes_[node_index].set_parent(parent);
	nodes_[node_index].set_label(trie_->label(trie_index));

	UInt32 value;
	if (trie_->get_value(trie_index, &value))
		enqueue_candidate(node_index, value);

	return node_index;
}

inline void ValueOrderCompleter::enqueue_node(UInt32 node_index)
{
	assert(node_index < nodes_.size());

	if (nodes_[node_index].is_queued())
		return;

	node_queue_.push_back(node_index);
	nodes_[node_index].set_is_queued();
}

inline void ValueOrderCompleter::enqueue_candidate(
	UInt32 node_index, UInt32 value)
{
	assert(node_index < nodes_.size());

	UInt32 candidate_index = candidate_queue_.size();
	candidate_queue_.resize(candidate_index + 1);

	candidate_queue_[candidate_index].set_index(node_index);
	candidate_queue_[candidate_index].set_value(value);

	UInt32 index = candidate_index;
	while (index != 0)
	{
		UInt32 parent_index = (index - 1) / 2;
		if (candidate_queue_[index] < candidate_queue_[parent_index])
			break;

		std::swap(candidate_queue_[index], candidate_queue_[parent_index]);
		index = parent_index;
	}
}

inline void ValueOrderCompleter::dequeue_candidate()
{
	assert(candidate_queue_.size() > 0);

	const ValueOrderCompleterCandidate &last = candidate_queue_.back();

	UInt32 parent_index = 0;
	UInt32 index = (parent_index * 2) + 1;
	while (index + 1 < candidate_queue_.size())
	{
		if (index + 2 < candidate_queue_.size() &&
			candidate_queue_[index] < candidate_queue_[index + 1])
			++index;
		if (candidate_queue_[index] < last)
			break;

		candidate_queue_[parent_index] = candidate_queue_[index];
		parent_index = index;
		index = (parent_index * 2) + 1;
	}

	candidate_queue_[parent_index] = last;
	candidate_queue_.resize(candidate_queue_.size() - 1);
}

inline const ValueOrderCompleterCandidate &
	ValueOrderCompleter::top_candidate()
{
	assert(candidate_queue_.size() > 0);

	return candidate_queue_.front();
}

}  // namespace sumire

#endif  // SUMIRE_VALUE_ORDER_COMPLETER_IN_H
