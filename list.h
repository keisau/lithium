#ifndef __LITHIUM_LIST_H__
#define __LITHIUM_LIST_H__
#include "lithium.h"
#include "types.h"
namespace li
{
#pragma pack (push, 4)
	struct list_head
	{
		list_head *prev, *next;
		list_head () { prev = next = this; }
		~list_head () {
			prev->next = next;
			next->prev = prev;
		}
	};

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
						p != tail && ({ nx = p->next; true; });
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
#pragma pack (pop)

	/**
	 * Iterator
	 */
#pragma pack (push, 4)
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
#pragma pack (pop)

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
