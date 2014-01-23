#ifndef __LITHIUM_HASH_H__
#define __LITHIUM_HASH_H__

#include <string.h>			// memcpy, etc
#include "types.h"

#define _PRIME_MASK		0xf
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
#define GOLDEN_RATIO_PRIME_32 0x9e370001UL
#define _HASH_SHIFT_MASK		0x1f

/**
 * Hash 32-bit integers to an integer of the designated bits,
 * copied from linux kernel.
 */
static inline u32 hash_32(u32 val, unsigned int bits)
{
	/* On some cpus multiply is faster, on others gcc will do shifts */
	u32 retval = val * GOLDEN_RATIO_PRIME_32;

	/* High bits are more random, so use them. */
	return retval >> (32 - bits);
}

/**
 * Hash 64-bit integers to an integer of the designated bits,
 * copied from linux kernel.
 */
static inline u64 hash_64(u64 val, unsigned int bits)
{
	u64 retval = val;

	/*  Sigh, gcc can't optimise this alone like it does for 32 bits. */
	u64 n = retval;
	n <<= 18;
	retval -= n;
	n <<= 33;
	retval -= n;
	n <<= 3;
	retval += n;
	n <<= 3;
	retval -= n;
	n <<= 4;
	retval += n;
	n <<= 2;
	retval += n;

	/* High bits are more random, so use them. */
	return retval >> (64 - bits);
}

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

static inline index_t _hash (const u8 *s, u32 len) 
{
	index_t retval;
	u32 size = len / sizeof (u64);
	u32 rem = len % sizeof (u64);
	u8 *p = (u8*) s + size * sizeof (u64);
	retval = _hash ((u64*) s, size);
	for (u32 i = 0; i < rem; ++i)
		retval ^= p[i] << (i << 3 /* size of a byte */);
	return retval;
}

template <typename _KeyType>
static inline index_t _hash (const _KeyType &key)
{
	index_t retval = 0;
	u32 size = sizeof (_KeyType) / sizeof (u64);
	u32 len = sizeof (key);
	u32 rem = len % sizeof (u64);

	retval ^= _hash ((u64*) &key, size);
	
	// handle trailing bytes
	u8 *p = (u8*) (((u64*) &key) + size);
	for (u32 i = 0; i < rem; ++i)
		retval ^= p[i] << (i << 3 /* size of a byte */);

	return retval;
}

#endif // __LITHIUM_HASH_H__
