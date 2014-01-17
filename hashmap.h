#ifndef __LITHIUM_HASHMAP_H__
#define __LITHIUM_HASHMAP_H__

#include "lithium.h"
#include "hash.h"
#include "list.h"
#include "iterator.h"
namespace li
{
	template <typename _KeyType, typename _ValType, class _Traits = li::traits <_KeyType> >
		class hashmap
		{
			// typedefs
		protected:
			typedef typename _Traits::ptr_t				__hash_key_t;
			struct hash_node
			{
				_KeyType			key;
				_ValType			value;
				list_head			slot_head;
				list_head			head;
				hash_node () {}
				hash_node (_KeyType key, _ValType value) :
					key(key), value(value) {}
			};

		public:
			typedef li::list_head_iterator <hash_node, _ValType> iterator;
			friend class li::list_head_iterator <hash_node, _ValType>;

			// public methods
		public:
			hashmap () : _size (4096) { _post_ctor (); }

			hashmap (size_t size) : _size (size) { _post_ctor (); }

			~hashmap () {}

			li::pair <iterator, bool> insert (const _KeyType &_key, const _ValType &val)
			{
				li::pair <iterator, bool> retval;
				__hash_key_t key = _Traits::get_key (_key);
				index_t _x = _hash ((u8 *)key, _Traits::get_size (key)) % _size;
				hash_node *node;

				/* list_for_each_entry */
				for (list_head *root = _table + _x, *p = root->next;
						p != root; 
						p = p->next)
				{
					node = container_of (p, hash_node, slot_head);
					if (node->key == _key) {
						retval.first = iterator (&node->head);
						retval.second = false;
						goto out;
					}
				}

				/* key not found, insert */
				node = new hash_node (_key, val);
				list_insert (_table + _x, &node->slot_head);
				list_insert (&head, &node->head);
				retval.first = iterator (&node->head);
				retval.second = true;
out:
				return retval;	
			}

			iterator find (const _KeyType &_key)
			{
				__hash_key_t key = _Traits::get_key (_key);
				index_t _x = _hash ((u8 *)key, _Traits::get_size (key)) % _size;
				hash_node *node;

				/* list_for_each_entry */
				for (list_head *root = _table + _x, *p = root->next;
						p != root; 
						p = p->next)
				{
					node = container_of (p, hash_node, slot_head);
					// found
					if (node->key == _key) {
						return iterator (&node->head);
					}
				}

				// not found
				return end ();
			}

			_ValType& operator[] (const _KeyType &key) { return *(find (key)); }

			iterator begin () {
				return iterator (head.next);
			}

			iterator end () {
				return iterator (&tail);
			}

			// protected methods
		protected:
			/**
			 * post-constructor initialization - run after constructor's
			 * initialization list
			 */
			void _post_ctor () {
				_table = new list_head [_size] ();
				list_insert (&head, &tail);
			}

			// attributes
		protected:
			size_t					_size;
			list_head				*_table;
			list_head				head;
			list_head				tail;
		};
}
#endif // __LITHIUM_HASHMAP_H__
