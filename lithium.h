#ifndef __LITHIUM_H__
#define __LITHIUM_H__

#include "types.h"

// lithium library templates
namespace li
{
	template <typename _Type>
		struct _naive_type_trait 
		{
			typedef _Type			type_t;
			typedef _Type			*ptr_t;
			type_t operator() (_Type _in) {
				return _in;
			}
		};

#pragma pack (push, 1)
	template <typename _Type1, typename _Type2>
		struct pair
		{
			_Type1		first;
			_Type2		second;
		};

#pragma pack (pop)
}

// utility functions
namespace li
{
	static inline index_t div_round_up (index_t ll, index_t d)
	{
		index_t tmp = ll + d - 1;
		return tmp / d;
	}
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
