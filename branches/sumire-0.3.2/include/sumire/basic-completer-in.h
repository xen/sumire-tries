#ifndef SUMIRE_BASIC_COMPLETER_IN_H
#define SUMIRE_BASIC_COMPLETER_IN_H

#include <cassert>

namespace sumire {

inline bool BasicCompleter::start(const TrieBase &trie, UInt32 index)
{
	if (trie.num_units() == 0)
		return false;

	assert(index < trie.num_units());

	clear();

	trie_ = &trie;
	key_.push_back('\0');
	index_stack_.push_back(index);

	return true;
}

inline bool BasicCompleter::next()
{
	assert(trie_ != NULL);

	if (index_stack_.empty())
		return false;

	if (value_ != TrieBase::INVALID_VALUE)
	{
		if (!find_next())
			return false;
	}

	return find_value();
}

inline const char *BasicCompleter::key() const
{
	assert(key_.size() > 0);

	return reinterpret_cast<const char *>(&key_[0]);
}

inline void BasicCompleter::clear()
{
	trie_ = NULL;
	key_.clear();
	index_stack_.clear();
	value_ = TrieBase::INVALID_VALUE;
}

inline bool BasicCompleter::find_next()
{
	assert(index_stack_.size() > 0);

	UInt32 child_index = trie_->child(index_stack_.back());
	if (child_index != 0)
	{
		push_index(child_index);
		return true;
	}

	while (index_stack_.size() > 1)
	{
		UInt32 sibling_index = trie_->sibling(index_stack_.back());
		pop_index();
		if (sibling_index != 0)
		{
			push_index(sibling_index);
			return true;
		}
	}

	return false;
}

inline bool BasicCompleter::find_value()
{
	assert(index_stack_.size() > 0);

	UInt32 index = index_stack_.back();
	while (!trie_->get_value(index, &value_))
	{
		index = trie_->child(index);
		if (index == 0)
			return false;
		push_index(index);
	}
	return true;
}

inline void BasicCompleter::push_index(UInt32 index)
{
	key_.back() = trie_->label(index);
	key_.push_back('\0');
	index_stack_.push_back(index);
}

inline void BasicCompleter::pop_index()
{
	assert(key_.size() > 1);
	assert(index_stack_.size() > 0);

	key_.resize(key_.size() - 1);
	key_.back() = '\0';
	index_stack_.resize(index_stack_.size() - 1);
}

}  // namespace sumire

#endif  // SUMIRE_BASIC_COMPLETER_IN_H
