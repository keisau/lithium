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
 * Doubly-ended list utility
 */

/**
 * Some codes come from linux kernel
 *
 * You may found the original code at:
 * $(linux_src)/include/linux/list.h
 */

#ifndef __LITHIUM_LIST_H__
#define __LITHIUM_LIST_H__

#include "lithium.h"
#include "types.h"

namespace li
{
	struct list_head
	{
		list_head *prev, *next;
		list_head () { prev = next = this; }
		~list_head () {
			prev->next = next;
			next->prev = prev;
		}
	};

	/**
	 * copied from linux kernel - general list insert
	 */
	static inline void __list_insert (list_head *item,
			list_head *prev,
			list_head *next)
	{
		next->prev = item;
		item->next = next;
		item->prev = prev;
		prev->next = item;
	}

	/**
	 * insert to head
	 */
	static inline void list_insert (list_head *head, list_head *item) {
		__list_insert (item, head, head->next);
	}

	/**
	 * insert to tail
	 */
	static inline void list_push_back (list_head *head, list_head *item) {
		__list_insert (item, head->prev, head);
	}

	/**
	 * delete
	 */
	static inline void list_delete (list_head *head) {
		head->prev->next = head->next;
		head->next->prev = head->prev;
		head->next = head->prev = NULL;
	}

	/**
	 * list container class - to be merged with generic list_head iterator
	 */
	template <typename _ValType>
		class list
		{
		public:
			class iterator;
			friend class iterator;

		protected:
			struct list_node
			{
			public:
				_ValType value;
				list_node *prev, *next;

			public:
				list_node () { prev = next = this; }
				list_node (_ValType value) : value (value) { prev = next = this; }
			};

		public:
			list ()
			{
				head = tail = new list_node ();
			}

			~list ()
			{
				// list_for_each_entry_safe
				for (list_node *p = head, *nx;
						p != tail &&
#ifndef _WIN32
						({ nx = p->next; true; });
#else
						((nx = p->next) || true);
#endif
						p = nx)
				{
					delete p;
				}
				delete tail;
			}

			iterator insert (_ValType value);
			iterator begin () { return iterator (head); }
			iterator end () { return iterator (tail); }

		protected:
			list_node *head, *tail;
		};

	/**
	 * Iterator
	 */
	template <typename _ValType>
		class list<_ValType>::iterator : public std::iterator <std::bidirectional_iterator_tag,_ValType>
		{
			friend class list<_ValType>;
		protected:
			list_node *p;

		public:
			// public constructors
			iterator () : p (NULL) {}
			iterator (const iterator &iter) : p (iter.p) {}
		protected:
			// protected constructor
			iterator (list_node *p) : p (p) {}

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
				p = p->prev;
				return *this;
			}

			iterator operator-- (int)
			{
				iterator retval = iterator (*this);
				p = p->prev;
				return retval;
			}

			bool operator== (const iterator &rvalue) const { return p == rvalue.p; }
			bool operator!= (const iterator &rvalue) const { return p != rvalue.p; }
			_ValType& operator* () const { return p->value; }
			_ValType* operator-> () const { return &p->value; }
		};

	template <typename _ValType>
		typename list<_ValType>::iterator list<_ValType>::insert (_ValType value)
		{
			list_node *node = new list_node (value);
			node->next = head;
			node->prev = tail;
			tail->next = node;
			head->prev = node;
			head = node;
			return iterator (node);
		}
} // namespace li
#endif // __LITHIUM_LIST_H__
