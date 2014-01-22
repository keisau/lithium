#ifndef __LITHIUM_STRING_H__
#define __LITHIUM_STRING_H__

#include <string.h>
#include <string>

namespace li
{
	using std::string;
	template <>
	struct traits <string>
	{
		typedef const char			*type_t;
		typedef const char			*ptr_t;

		static size_t get_size (const string &_in) {
			return _in.length();
		}

		static type_t get_key (const string &_in) {
			return _in.c_str();
		}

		static ptr_t get_key_ptr (const string &_in) {
			return _in.c_str();
		}
	};
}
#endif // __LITHIUM_STRING_H__
