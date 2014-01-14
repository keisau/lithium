#ifndef __LITHIUM_HASHMAP_H__
#define __LITHIUM_HASHMAP_H__

#include "lithium.h"
#include "hash.h"
#include "hlist.h"

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
			typedef li::pair <_KeyType, _ValType>			item_t;
			typedef typename _Traits::type_t				hash_key_t;

			// public methods
		public:
			hashmap () : _size (4096) {
				_table = new hlist <item_t> [4096];
			}

			hashmap (size_t size) : _size (size) {
				_table = new hlist <item_t> [_size];
			}

			~hashmap () {}

			iterator insert (_KeyType key, _ValType val)
			{
			}

			// protected methods
		protected:
			index_t get_hash (hash_key_t hash_key) {
				index_t retval = _hash (hash_key);
				return retval % _size;
			}

			// attributes
		protected:
			size_t					_size;
			li::hlist <item_t>		*_table;
		};
}
#endif // __LITHIUM_HASHMAP_H__
