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

			typedef typename _Traits::ptr_t				__hash_key_t;

		public:
			typedef li::list_head_iterator <hash_node, _ValType> iterator;
			friend class li::list_head_iterator <hash_node, _ValType>;

			// public methods
		public:
			hashmap () : _size (4096) { _post_ctor (); }

			hashmap (size_t size) : _size (size) { _post_ctor (); }

			~hashmap () {}

			void insert (_KeyType _key, _ValType val)
			{
				__hash_key_t key = _Traits::key(_key);
				hash_node *node = new hash_node (_key, val);
				index_t _x = _hash ((u8 *)key, _Traits::size (key)) % _size;

				// table slot
				list_insert (_table + _x, &node->slot_head);

				// iterator head
				list_insert (&head, &node->head);
			}

			pair <_ValType, bool> find (_KeyType _key)
			{
				__hash_key_t key = _Traits::(_key);
				index_t _x;
			}

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
