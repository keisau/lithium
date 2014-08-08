#ifndef __LITHIUM_TYPES_H__
#define __LITHIUM_TYPES_H__

#include <stdint.h>			// [u]int[8,16,32,64]_t, etc
#include <utility>			// std::pair, etc
#include <iterator>			// std::iterator, etc
#include <functional>		// std::less<T>, etc

/**
 * typedefs
 */
typedef int64_t				s64;
typedef int32_t				s32;
typedef int16_t				s16;
typedef int8_t				s8;
typedef uint64_t			u64;
typedef uint32_t			u32;
typedef uint16_t			u16;
typedef uint8_t				u8;
typedef u64					index_t;		// indicing key for radix tree

#endif // __LITHIUM_TYPES_H__
