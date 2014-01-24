#ifndef __LITHIUM_HASHMAP_H__
#define __LITHIUM_HASHMAP_H__

#include "lithium.h"
#include "hash.h"
#include "list.h"
#include "iterator.h"
#include "string.h"

namespace li
{
template <typename _KeyType, 
		 typename _ValType, 
	 class _Equal = std::equal_to<_KeyType>,
 class _Traits = li::traits <_KeyType> >
 class hashmap
{
	// typedefs
protected:
	typedef typename _Traits::ptr_t				__hash_ptr_t;
	typedef li::pair<_KeyType, _ValType> val_t;

	/**
	 * A POD struct, hope it is safe for using container_of
	 */
	struct hash_node
	{
		val_t				value;
		list_head			slot_head;			// chaining list_head
		list_head			head;				// for iterator
		hash_node () {}
		hash_node (_KeyType key, _ValType val) {
			value.first = key;
			value.second = val;
		}
	};
public:
	// iterator
	typedef li::list_head_iterator <hash_node, val_t> iterator;
	friend struct li::list_head_iterator <hash_node, val_t>;

	typedef li::const_list_head_iterator <hash_node, val_t> const_iterator;
	friend struct li::const_list_head_iterator <hash_node, val_t>;

	// public methods
public:
	// default constructor
	hashmap () : _capacity (4096), _size (0) { __ctor (); }

	hashmap (size_t capacity) : _capacity (capacity), _size (0) { __ctor (); }

	// copy constructor
	hashmap (const hashmap &_map) : _size (_map.size ()),
			_capacity (_map.capacity ())
			{
				__ctor ();

				for (const list_head *root = &_map._head, *p = root->next;
						p != root;
						p = p->next)
				{
					hash_node *node = container_of (p, hash_node, head);
					val_t value = node->value;
					insert (value.first, value.second);
				}
			}

	~hashmap () {
		clear ();
		delete [] _table;
	}

	const_iterator begin () const { return const_iterator (_head.next); }
	iterator begin () {	return iterator (_head.next); }

	const_iterator end () const { return const_iterator (&_head); }
	iterator end () { return iterator (&_head); }

	bool empty () const { return _size == 0; }

	void clear () {
		// list_for_each_entry_safe
		for (list_head *root = &_head, *p = root->next, *nx;
				p != root && ((nx = p->next) || 1);
				p = nx)
		{
			hash_node *node = container_of (p, hash_node, head);
			delete node;
			--_size;
		}
	}

	li::pair <iterator, bool> insert (const _KeyType &_key, const _ValType &val)
	{
		hash_node *node;
		list_head *root, *lhead;
		lhead = __find (_key, root);

		/* key exists in the map, don't insert */
		if (lhead != &_head)
			return li::make_pair (iterator (lhead), false);

		/* key not found, insert */
		node = new hash_node (_key, val);
		list_insert (root, &node->slot_head);
		list_insert (&_head, &node->head);
		++_size;
		return li::make_pair (iterator (&node->head), true);
	}

	iterator find (const _KeyType &_key)
	{
		list_head *root;
		return iterator (__find (_key, root)) ;
	}

	const_iterator find (const _KeyType &_key) const
	{
		list_head *root;
		return const_iterator (__find (_key, root));
	}

	void erase (iterator iter)
	{
		if (iter.p != &_head) {
			list_head *p = iter.p;
			hash_node *node = container_of (p, hash_node, head);
			delete node;
			--_size;
		}
	}

	size_t erase (const _KeyType &key)
	{
		iterator iter = find (key);
		if (iter != end())
		{
			erase (iter);
			return 1;
		}
		return 0;
	}

	hashmap& operator= (const hashmap &_map) {
		size_t _cap = _map.capacity ();
		clear ();
		if (_capacity != _cap) {
			delete [] _table;
			_capacity = _cap;
			_table = new list_head [_capacity] ();
		}

		const list_head *root = &_map._head;
		for (list_head *p = root->next;
				p != root;
				p = p->next)
		{
			hash_node *node = container_of (p, hash_node, head);
			val_t value = node->value;
			insert (value.first, value.second);
		}

		return *this;
	}

	_ValType& operator[] (const _KeyType &key) { 
		iterator it = insert (key, _ValType ()).first;
		return it->second;
	}

	size_t capacity () const { return _capacity; }
	size_t size () const { return _size; }

	// protected methods
protected:
	/**
	 * post-constructor initialization - run after constructor's
	 * initialization list
	 */
	void __ctor () {
		_table = new list_head [_capacity] ();
	}

	/**
	 * find a key in the hashmap, setting root list_head for insert () without
	 * sacrificing performance and design
	 */
	list_head *__find (const _KeyType &_key, list_head *&root)
	{
		__hash_ptr_t keyptr = _Traits::get_key_ptr (_key);
		index_t _x = _hash ((u8 *)keyptr, _Traits::get_size (_key)) % _capacity;
		hash_node *node;

		/* list_for_each_entry */
		root = _table + _x;
		for (list_head *p = root->next;
				p != root; 
				p = p->next)
		{
			node = container_of (p, hash_node, slot_head);
			/**
			 * depends on equality operator of the key type
			 */
			if (_equal (node->value.first, _key)) {
				// found
				return &node->head;
			}
		}

		return &_head;
	}

	// attributes
protected:
	size_t					_size;
	size_t					_capacity;
	list_head				*_table;
	list_head				_head;
	_Equal					_equal;
};
}
#endif // __LITHIUM_HASHMAP_H__
