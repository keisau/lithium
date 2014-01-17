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
#ifndef __LITHIUM_ITERATOR_H__
#define __LITHIUM_ITERATOR_H__

#include <iterator>
namespace li
{
	/**
	 * list_head_iterator - use hacky container_of macro to implement high
	 * memory efficiency iterator
	 *
	 * Protocol: 
	 * 1. _Class must include a member variable head of type list_head.
	 * 2. _Class must include a member variable value of type _ValType.
	 *
	 * Assumption:
	 * 1. _Class is a POD type (for using container_of)
	 * 2. _ValType provides the operators required
	 */
	template <class _Class, typename _ValType>
	struct list_head_iterator :
		public std::iterator <std::bidirectional_iterator_tag, _ValType>
	{
		list_head	*p;

		// constructors
		list_head_iterator () : p (NULL) {}
		list_head_iterator (const list_head_iterator &iter) : p (iter.p) {}
		list_head_iterator (list_head *p) : p (p) {}

		// operators
		list_head_iterator& operator= (const list_head_iterator &iter) {
			p = iter.p;
			return *this;
		}

		list_head_iterator& operator++ () {
			p = p->next;
			return *this;
		}

		list_head_iterator operator++ (int) {
			list_head_iterator retval = list_head_iterator (*this);
			p = p->next;
			return retval;
		}

		list_head_iterator& operator-- () {
			p = p->prev;
			return *this;
		}

		list_head_iterator operator-- (int) {
			list_head_iterator retval = list_head_iterator (*this);
			p = p->prev;
			return retval;
		}

		bool operator== (const list_head_iterator &rvalue) const { return p == rvalue.p; }
		bool operator!= (const list_head_iterator &rvalue) const { return p != rvalue.p; }
		_ValType& operator* () const {
			_Class *obj = container_of (p, _Class, head);
			return obj->value;
		}

		_ValType* operator-> () const {
			_Class *obj = container_of (p, _Class, head);
			return &obj->value;
		}
	};
}

#endif // __LITHIUM_ITERATOR_H__
