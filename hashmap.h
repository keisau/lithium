#ifndef __LITHIUM_HASHMAP_H__
#define __LITHIUM_HASHMAP_H__

#include "lithium.h"
#include "hash.h"
#include "hlist.h"
#include "list.h"

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
			struct item_node
			{
				_KeyType			key;
				_ValType			value;
				list_head			head;
			};

			typedef typename _Traits::type_t				__hash_key_t;

			// public methods
		public:
			hashmap () : _size (4096) {
				_table = new hlist <item_node> [4097];
				_table [4096] = NULL;
			}

			hashmap (size_t size) : _size (size) {
				_table = new hlist <item_node> [_size + 1];
				_table [size] = NULL;
			}

			~hashmap () {}

			void insert (_KeyType _key, _ValType val)
			{
				__hash_key_t key = _Traits ()(_key);
				index_t _x = get_hash (_key);
				_table [_x].insert (make_pair (key, val));
			}

			bool find (_KeyType _key)
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
			li::hlist <item_node>		*_table;
		};
}
#endif // __LITHIUM_HASHMAP_H__
