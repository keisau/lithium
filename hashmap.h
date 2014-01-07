#ifndef __LITHIUM_HASHMAP_H__
#define __LITHIUM_HASHMAP_H__
#include "hash.h"
#include "hlist.h"
#include "radix_tree.h"

template <typename _KeyType,
		 typename _ValType,
		 class _Less = less<_KeyType>>
class hashmap
{
public:
	class iterator;
	friend class iterator;

protected:
	typedef std::pair <_KeyType, _ValType> _ItemType;

public:
	hashmap () {}
	~hashmap () {}

protected:

protected:
	radix_tree <hlist> _RadixMap;
};
#endif // __LITHIUM_HASHMAP_H__
