#ifndef __LITHIUM_LIST_H__
#define __LITHIUM_LIST_H__

namespace li
{
#pragma pack (push, 4)
template <typename _ValType>
class list
{
public:
	class iterator;
	friend class iterator;

protected:
	struct list_node
	{
		_ValType value;
		list_node *next, *prev;
		list_node () 
		{
			next = prev = this;
		}
		list_node (_ValType value) : value (value) {}
	};

public:
	list ()
	{
		head = tail = new list_node ();
	}

	~list ()
	{
		// list_for_each_entry_safe
		for (list_node *p = first, *nx;
				p != NULL && ({ nx = p->next; true; });
				p = nx)
		{
			delete p;
		}
	}

	iterator insert (_ValType value);
	iterator begin () { return iterator (head); }
	iterator end () { return iterator (tail); }

protected:
	list_node *head, *tail;
};
#pragma pack (pop)

/**
 * Iterator
 */
#pragma pack (push, 4)
template <typename _ValType>
class list<_ValType>::iterator : public std::iterator <std::bidirectional_iterator_tag,_ValType>
{
	friend class list<_ValType>;
protected:
	list_node *p;

public:
	// public constructors
	iterator () : p (NULL) {}
	iterator (const iterator &iter) : p (iter.p) {}
protected:
	// protected constructor
	iterator (list_node *p) : p (p) {}
	
public:
	iterator& operator= (const iterator &iter)
	{
		p = iter.p;
		return *this;
	}

	iterator& operator++ () 
	{
		p = p->next;
		return *this;
	}

	iterator operator++ (int)
	{
		iterator retval = iterator (*this);
		p = p->next;
		return retval;
	}

	iterator& operator-- () 
	{
		p = p->prev;
		return *this;
	}

	iterator operator-- (int)
	{
		iterator retval = iterator (*this);
		p = p->prev;
		return retval;
	}

	bool operator== (const iterator &rvalue) const { return p == rvalue.p; }
	bool operator!= (const iterator &rvalue) const { return p != rvalue.p; }
	_ValType& operator* () const { return p->value; }
	_ValType* operator-> () const { return &p->value; }
};
#pragma pack (pop)

	template <typename _ValType>
typename list<_ValType>::iterator list<_ValType>::insert (_ValType value)
{
	list_node *node = new list_node (value);
	node->next = first;
	if (first)
		first->pprev = &node->next;

	first = node;
	node->pprev = &first;
	return iterator (node);
}
} // namespace li
#endif // __LITHIUM_LIST_H__
