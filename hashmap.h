#ifndef __LITHIUM_HASHMAP_H__
#define __LITHIUM_HASHMAP_H__

#include "lithium.h"
#include "hash.h"
#include "list.h"
#include "iterator.h"

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

			/**
			 * A POD struct, hope it is safe for using container_of
			 */
			struct hash_node
			{
				li::pair<_KeyType, _ValType>			value;
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
			typedef li::pair<_KeyType, _ValType> val_t;
			typedef li::list_head_iterator <hash_node, val_t> iterator;
			friend struct li::list_head_iterator <hash_node, val_t>;

			// public methods
		public:
			hashmap () : capacity (4096), size (0) { _post_ctor (); }

			hashmap (size_t capacity) : capacity (capacity), size (0) { _post_ctor (); }

			~hashmap () {
				list_head *root = &head;
				for (iterator it = begin (); it != end (); ++it)
					erase (it);
				delete [] table;
			}

			li::pair <iterator, bool> insert (const _KeyType &_key, const _ValType &val)
			{
				hash_node *node;
				list_head *root, *lhead;
				lhead = _find (_key, root);

				/* key exists in the map, don't insert */
				if (lhead != &head)
					return li::make_pair (iterator (lhead), false);

				/* key not found, insert */
				node = new hash_node (_key, val);
				list_insert (root, &node->slot_head);
				list_insert (&head, &node->head);
				++size;
				return li::make_pair (iterator (&node->head), true);
			}

			iterator find (const _KeyType &_key)
			{
				list_head *root;
				return iterator (_find (_key, root));
			}

			void erase (const iterator &iter)
			{
				if (iter.p != &head) {
					hash_node *node = container_of (iter.p, hash_node, head);
					delete node;
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

			hashmap& operator= (hashmap &_map) {
				for (iterator it = _map.begin (); it != _map.end (); ++it) {
					val_t value = *it;
					insert (value.first, value.second);
				}
				size = _map.size;
				return *this;
			}

			_ValType& operator[] (const _KeyType &key) { 
				iterator it = find (key); 
				if (it == end ())
					it = insert (key, _ValType ()).first;
				return it->second;
			}

			iterator begin () {
				return iterator (head.next);
			}

			iterator end () {
				return iterator (&head);
			}

			// protected methods
		protected:
			/**
			 * post-constructor initialization - run after constructor's
			 * initialization list
			 */
			void _post_ctor () {
				table = new list_head [capacity] ();
			}

			/**
			 * find a key in the hashmap, setting root list_head for insert () without
			 * sacrificing performance and design
			 */
			list_head *_find (const _KeyType &_key, list_head *&root)
			{
				__hash_ptr_t keyptr = _Traits::get_key_ptr (_key);
				index_t _x = _hash ((u8 *)keyptr, _Traits::get_size (_key)) % capacity;
				hash_node *node;

				/* list_for_each_entry */
				root = table + _x;
				for (list_head *p = root->next;
						p != root; 
						p = p->next)
				{
					node = container_of (p, hash_node, slot_head);
					/**
					 * depends on equality operator of the key type
					 */
					if (equal (node->value.first, _key)) {
						// found
						return &node->head;
					}
				}

				return &head;
			}

			// attributes
		protected:
			size_t					size;
			size_t					capacity;
			list_head				*table;
			list_head				head;
			_Equal					equal;
		};
}
#endif // __LITHIUM_HASHMAP_H__
