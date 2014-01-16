#ifndef __LITHIUM_HASHMAP_H__
#define __LITHIUM_HASHMAP_H__

#include "lithium.h"
#include "hash.h"
#include "list.h"
#include "list.h"
#include <stdio.h>
namespace li
{
	template <typename _KeyType, typename _ValType, class _Traits = li::_naive_type_trait <_KeyType> >
		class hashmap
		{
		public:
			class iterator;
			friend class iterator;

			// typedefs
		protected:
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

			typedef typename _Traits::type_t				__hash_key_t;

			// public methods
		public:
			hashmap () { 
				list_insert (&head, &tail);
				printf ("%d\n", offsetof (hash_node, head)); 
			}

			hashmap (size_t size) : _size (size) {
				_table = new list_head [_size] ();
			}

			~hashmap () {}

			void insert (_KeyType _key, _ValType val)
			{
				__hash_key_t key = _Traits ()(_key);
				index_t _x = get_hash (_key);
				hash_node *node = new hash_node (_key, val);

				// table slot
				list_insert (_table + _x, &node->slot_head);

				// iterator head
				list_insert (&head, &node->head);
			}

			pair <_ValType, bool> find (_KeyType _key)
			{
				__hash_key_t key = _Traits ()(_key);
				index_t _x = get_hash (_key);
			}

			// protected methods
		protected:
			index_t get_hash (__hash_key_t hash_key) {
				index_t retval = _hash (hash_key);
				return retval % _size;
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
