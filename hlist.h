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
 * hlist implementation based on linux kernel's hlist_head & hlist_head.
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
		struct hlist_head
		{
			_ValType value;
			hlist_head *next, **pprev;
			hlist_head () : next (NULL), pprev (NULL) {}
			hlist_head (_ValType value) : next (NULL), pprev (NULL), value (value) {}
			~hlist_head () {
				if (pprev) {
					hlist_head *node = container_of (pprev, hlist_head, next);
					node->next = next;
				}
				if (next)
					next->pprev = pprev;
			}
		};
#pragma pack (pop)

#pragma pack (push, 4)
	template <typename _ValType>
		class hlist
		{
		public:
			class iterator;
			friend class iterator;

		protected:

		public:
			hlist () : first (NULL) {}
			~hlist ()
			{
				// hlist_for_each_entry_safe
				for (hlist_head<_ValType> *p = first, *nx;
						p != NULL && 
#ifndef _WIN32
						({ nx = p->next; true; });
#else
						((nx = p->next) || true);
#endif
						p = nx)
				{
					delete p;
				}
			}

			iterator insert (_ValType value);
			iterator erase (iterator position);

			iterator begin () { return iterator (first); }
			iterator end () { return iterator (NULL); }
			iterator tail ()
			{
				hlist_head<_ValType> *retval = first;
				if (retval)
					while (retval->next) retval = retval->next;
				return iterator (retval);
			}

		protected:
			hlist_head<_ValType> *first;
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
			hlist_head<_ValType> *p;

		public:
			// public constructors
			iterator () : p (NULL) {}
			iterator (const iterator &iter) : p (iter.p) {}
		protected:
			// protected constructor
			iterator (hlist_head<_ValType> *p) : p (p) {}

		public:
			iterator& operator= (const iterator &iter) {
				p = iter.p;
				return *this;
			}

			iterator& operator++ () {
				p = p->next;
				return *this;
			}

			iterator operator++ (int) {
				iterator retval = iterator (*this);
				p = p->next;
				return retval;
			}

			iterator& operator-- () {
				if (p != NULL && p->pprev != NULL)
					p = container_of (p->pprev, hlist_head, next);
				else
					p = NULL;
				return *this;
			}

			iterator operator-- (int) {
				iterator retval = iterator (*this);
				if (p != NULL && p->pprev != NULL)
					p = container_of (p->pprev, hlist_head, next);
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
			hlist_head<_ValType> *node = new hlist_head<_ValType> (value);
			node->next = first;
			if (first)
				first->pprev = &node->next;

			first = node;
			return iterator (node);
		}
		
	template <typename _ValType>
		typename hlist<_ValType>::iterator hlist<_ValType>::erase (iterator position)
		{
			iterator before = position;
			++position;

			// special case : hlist's head node doesn't have a valid pprev
			if (before == first)
				first = first->next;
			delete before.p;
			return position;
		}
} // namespace li

#endif // __LITHIUM_HLIST_H__
