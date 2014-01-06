#ifndef __SUBLIME_HASH_H__
#define __SUBLIME_HASH_H__

#include <string.h>			// memcpy, etc
#include "types.h"

// 16 selected primes
const u32 _primes[16] = {
	28472401U,
	36525343U,
	49979687U,
	67867967U,
	72315377U,
	93245533U,
	160481183U,
	236887691U,
	443826839U,
	536870923U,
	736887719U,
	895829381U,
	1342342367U,
	2147483659U,
	3737267581U,
	4129478957U,
};
#define _PRIME_MASK		0xf

#define _HASH_SHIFT_MASK		0x1f
static inline index_t _hash (const u32 *s, u32 len)
{
	index_t retval = 0;
	for (u32 i = 0; i < len; ++i)
		retval ^= (u64) s[i] * _primes [i & _PRIME_MASK];
	
	return retval;
}

static inline index_t _hash (const u64 *s, u32 len)
{
	index_t retval = 0;
	for (u32 i = 0; i < len; ++i)
		retval ^= (s[i] >> (i & _HASH_SHIFT_MASK)) * _primes [i & _PRIME_MASK];
	
	return retval;
}

template <typename _KeyType>
static inline index_t _hash (const _KeyType &key)
{
	index_t retval = 0;
	u32 size = sizeof (_KeyType) / sizeof (u64);

	retval ^= _hash ((u64*) &key, size);
	
	// handle trailing bytes
	if (sizeof (_KeyType) > size * sizeof (u64))
	{
		u8 *p = (u8*) (((u64*) &key) + size);
		u32 bytesize = sizeof (_KeyType) % sizeof (u64);
		for (u32 i = 0; i < bytesize; ++i)
			retval ^= p[i] << (i * 8 /* size of a byte */);
	}
	
	return retval;
}

#endif // __SUBLIME_HASH_H__
