#ifndef __LITHIUM_H__
#define __LITHIUM_H__

#include "types.h"

// constants

// utility functions
static inline index_t div_round_up (index_t ll, index_t d)
{
	index_t tmp = ll + d - 1;
	return tmp / d;
}

// utility macros
/**
 * The container_of macro - modified to fit into non-gnu compilers without
 * the typeof keyword.
 */
#define container_of(ptr, type, member) ({							\
		const char *__mptr = (char*) (ptr);			\
		(type *) (__mptr - offsetof (type, member)); })

#endif // __LITHIUM_H__
