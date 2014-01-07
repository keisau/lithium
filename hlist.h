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

 * You should have received a copy of the GNU General Public License
 * along with lithium.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * hlist implementation based on linux kernel's hlist_head & hlist_node.
 *
 * Author: Pierre Saux
 * First commit date: 6-Jan-2014
 *
 * You may find the original version in:
 * 
 * Type definition: $(linux_src)/include/linux/types.h
 * hlist operation: $(linux_src)/include/linux/hlist.h
 */
#ifndef __LITHIUM_HLIST_H__
#define __LITHIUM_HLIST_H__

#include "lithium.h"
#include "types.h"

namespace li
{
#pragma pack (push, 4)
	template <typename _ValType>
		class hlist
		{
		public:
			class iterator;
			friend class iterator;

		protected:
			struct hlist_node
			{
				_ValType value;
				hlist_node *next, **pprev;
				hlist_node () : next (NULL), pprev (NULL) {}
				hlist_node (_ValType value) : value (value) {}
			};

		public:
			hlist () : first (NULL) {}
			~hlist ()
			{
				// hlist_for_each_entry_safe
				for (hlist_node *p = first, *nx;
						p != NULL && ({ nx = p->next; true; });
						p = nx)
				{
					delete p;
				}
			}

			iterator insert (_ValType value);
			iterator begin () { return iterator (first); }
			iterator end () { return iterator (NULL); }
			iterator tail ()
			{
				hlist_node *retval = first;
				if (retval)
					while (retval->next) retval = retval->next;
				return iterator (retval);
			}

		protected:
			hlist_node *first;
		};
#pragma pack (pop)

	/**
	 * Iterator
	 */
#pragma pack (push, 4)
	template <typename _ValType>
		class hlist<_ValType>::iterator : public std::iterator <std::bidirectional_iterator_tag,_ValType>
		{
			friend class hlist<_ValType>;
		protected:
			hlist_node *p;

		public:
			// public constructors
			iterator () : p (NULL) {}
			iterator (const iterator &iter) : p (iter.p) {}
		protected:
			// protected constructor
			iterator (hlist_node *p) : p (p) {}

		public:
			iterator& operator= (const iterator &iter)
			{
				p = iter.p;
				return *this;
			}

			iterator& operator++ () 
			{
				p = p->next;
				return *this;
			}

			iterator operator++ (int)
			{
				iterator retval = iterator (*this);
				p = p->next;
				return retval;
			}

			iterator& operator-- () 
			{
				if (p != NULL && p->pprev != NULL)
					p = container_of (p->pprev, hlist_node, next);
				else
					p = NULL;
				return *this;
			}

			iterator operator-- (int)
			{
				iterator retval = iterator (*this);
				if (p != NULL && p->pprev != NULL)
					p = container_of (p->pprev, hlist_node, next);
				else
					p = NULL;
				return retval;
			}

			bool operator== (const iterator &rvalue) const { return p == rvalue.p; }
			bool operator!= (const iterator &rvalue) const { return p != rvalue.p; }
			_ValType& operator* () const { return p->value; }
			_ValType* operator-> () const { return &p->value; }
		};
#pragma pack (pop)

	template <typename _ValType>
		typename hlist<_ValType>::iterator hlist<_ValType>::insert (_ValType value)
		{
			hlist_node *node = new hlist_node (value);
			node->next = first;
			if (first)
				first->pprev = &node->next;

			first = node;
			return iterator (node);
		}
} // namespace li

#endif // __LITHIUM_HLIST_H__
