/**
 * This file is part of lithium.
 *
 * lithium is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * lithium is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lithium.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Lithium utilities
 *
 * This header includes:
 * 1. traits for templates
 * 2. utility classes
 * 3. utility functions
 *
 * Some codes are inspired by the linux kernel:
 * Header: $(linux_src)/include/linux/kernel.h
 */

#ifndef __LITHIUM_H__
#define __LITHIUM_H__

#include "types.h"

// lithium library templates
namespace li
{
template <typename _Type>
struct traits 
{
	typedef _Type			type_t;
	typedef _Type			*ptr_t;

	static size_t get_size (const _Type &_in) {
		return sizeof (type_t);
	}

	static type_t get_key (const _Type &_in) {
		return (type_t) _in;
	}

	static ptr_t get_key_ptr (const _Type &_in) {
		return  (ptr_t) &_in;
	}
};

#pragma pack (push, 1)
template <typename _Type1, typename _Type2>
struct pair
{
	_Type1		first;
	_Type2		second;
	pair& operator= (const pair& pr) { 
		first = pr.first; second = pr.second;
		return *this;
	}

	bool operator== (const pair& rvalue) {
		return first == rvalue.first && second == rvalue.second;
	}

	bool operator!= (const pair& rvalue) {
		return !(*this == rvalue);
	}

	bool operator<  (const pair& rvalue) { 
		return first < rvalue.first 
		|| (rvalue.first >= first && second < rvalue.second); 
	}

	bool operator<= (const pair& rvalue) {
		return !(rvalue < *this);
	}

	bool operator>  (const pair& rvalue) {
		return rvalue < *this; 
	}

	bool operator>= (const pair& rvalue) {
		return !(*this < rvalue); 
	}
};
#pragma pack (pop)

	template <typename _Type1, typename _Type2>
li::pair <_Type1, _Type2> make_pair (_Type1 x, _Type2 y)
{
	li::pair<_Type1, _Type2> retval;
	retval.first = x;
	retval.second = y;
	return retval;
}
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
 * The offsetof macro - hoped to eliminate Winvalid-offsetof warning
 */
#undef offsetof
#define offsetof(type, member) ((size_t) &(((type *)0)->member))

/**
 * The container_of macro - modified to support compilers without
 * the typeof keyword.
 */
#ifndef _WIN32
#define container_of(ptr, type, member) ({						\
		const char *__mptr = (char*) (ptr);						\
		(type *) (__mptr - offsetof (type, member)); })
#else
#define container_of(ptr, type, member)							\
	((type *) ((char*) (ptr) - offsetof (type, member)))
#endif

#endif // __LITHIUM_H__
