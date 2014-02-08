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
 * string support header - include c string and stl string and lithium
 * supports
 *
 * Author: Pierre Saux
 */

/**
 * Include this file if you need std::string support in lithium
 */

#ifndef __LITHIUM_STRING_H__
#define __LITHIUM_STRING_H__

#include <string.h>
#include <string>

namespace li
{
	using std::string;
/**
 * Specialization for std::string for easier life
 */
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
