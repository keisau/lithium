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
 * Radix tree implementation based on linux kernel's radix tree.
 *
 * Author: Pierre Saux
 * First commit date: 3-Jan-2014
 *
 * You may find the original version in:
 * 
 * Header: $(linux_src)/include/linux/radix_tree.h
 * Source: $(linux_src)/lib/radix_tree.c
 */
#ifndef __LITHIUM_RADIX_TREE_H__
#define __LITHIUM_RADIX_TREE_H__

#include "lithium.h"
#include "types.h"

/**
 * Branch factor must be a power of 2. Default to 2^6 (copied from kernel)
 */
#define RT_BRANCH_FACTOR_BIT	(6)
#define RT_BRANCH_FACTOR		(1 << RT_BRANCH_FACTOR_BIT)
#define RT_BRANCH_INDEX_MASK	(RT_BRANCH_FACTOR - 1)

namespace li
{
	/**
	 * radix_tree container
	 */
#pragma pack (push, 4)
	template <typename _ValType>
		class radix_tree
		{
			// types
		public:
			class iterator;
			friend class iterator;

		protected:
			struct list_node;
			struct radix_node;

			// functions
		public:
			radix_tree ()
			{
				rt_root = new radix_node ();
				list_head.insert (&list_tail);

				// initialize max height
				max_height = (u32) (div_round_up (sizeof (index_t) << 3,
							RT_BRANCH_FACTOR_BIT));

				// initialize max index array
				u64 last = 1;
				for (u32 i = 0; i < max_height; ++i)
				{
					max_index[i] = last << RT_BRANCH_FACTOR_BIT;
					last = max_index[i];
					--max_index[i];
				}
			}

			// dfs
			~radix_tree ()
			{
				radix_node *node = rt_root, *slot;
				for (int i = 0; i < RT_BRANCH_FACTOR; ++i)
				{
					if (node)
					{
						slot = node->slots[i];
						if (slot)
							delete slot;
					}
				}
			}

			iterator begin () { return iterator (list_head.next); }
			iterator end () { return iterator (&list_tail); }

			std::pair<iterator, bool> insert (index_t key, _ValType val);
			iterator find (index_t key) const;
			u32 erase (const index_t &key);
			void erase (const iterator &iter);

			_ValType& operator[] (index_t key)
			{
				return (_ValType) *(find (key));
			}

		protected:
			radix_node * extend ()
			{	// only happen on the root
				radix_node * retval = new radix_node ();
				retval->height = rt_root->height + 1;
				retval->size = 1;
				retval->slots[0] = rt_root;
				rt_root->parent = retval;
				return retval;

			}
			void shrink (radix_node *node)		
			{	// only happen on the root
				while (node != rt_root && node->size == 0)
				{
					radix_node * parent = node->parent;
					parent->slots[node->offset] = NULL;
					delete node;
					node = parent;
				}
			}

			// member fields
		protected:
			radix_node		*rt_root;
			list_node		list_head;		// begin()
			list_node		list_tail;		// end()
			u32				max_height;
			index_t			max_index[sizeof(index_t) << 3 /* size of a byte */];
		};
#pragma pack (pop)

	/**
	 * Iterator
	 */
#pragma pack (push, 4)
	template <typename _ValType>
		class radix_tree<_ValType>::iterator :
		public std::iterator <std::bidirectional_iterator_tag,_ValType>
		{
			friend class radix_tree<_ValType>;
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

	/**
	 * radix tree doubly-ended linked list for iteration
	 */
#pragma pack (push, 4)
	template <typename _ValType>
		struct radix_tree<_ValType>::list_node
		{
		public:
			_ValType value;
			list_node *prev, *next;
			typename radix_tree<_ValType>::radix_node *container;

		public:
			list_node () { prev = next = this; }
			list_node (_ValType value) : value (value) { prev = next = this; }

			virtual ~list_node ()
			{
				prev->next = next;
				next->prev = prev;
			}
			void insert (list_node * node)
			{
				list_node * tmp = next;
				next = node;
				tmp->prev = node;

				node->prev = this;
				node->next = tmp;
			}
		};
#pragma pack (pop)

#define RT_INDIRECT_NODE		0
#define RT_DATA_NODE			1

	/**
	 * radix tree node
	 */
#pragma pack (push, 4)
	template <typename _ValType>
		struct radix_tree<_ValType>::radix_node
		{
		public:
			radix_node		*parent;	// parent slot
			u16				height;		// height of subtree
			u16				size;		// number of occupied slots
			u16				offset;		// parent->slots[offset] == this
			u16				flags;

			// linked list for O(n) stepwise iteration (BFS)
			typename radix_tree::list_node		node;

			radix_node		*slots [RT_BRANCH_FACTOR];

		public:
			radix_node () :	height (0),
			size (0),
			offset (0),
			flags (RT_INDIRECT_NODE)
			{
				parent = this;
				for (int i = 0; i < RT_BRANCH_FACTOR; ++i)
					slots[i] = NULL;
			}

			radix_node (_ValType value) : height (0),
			size (0),
			offset (0),
			flags (RT_DATA_NODE),
			node (list_node (value))
			{
				parent = this;
				for (int i = 0; i < RT_BRANCH_FACTOR; ++i)
					slots[i] = NULL;
			}
			
			// dfs delete
			~radix_node ()
			{
				radix_node *slot;
				if (flags == RT_INDIRECT_NODE)
					for (int i = 0; i < RT_BRANCH_FACTOR; ++i)
					{
						slot = slots[i];
						if (slot)
							delete slot;
					}
			}
		};
#pragma pack (pop)

	/**
	 * radix tree function definitions
	 */

	/**
	 * insert key-value pair
	 */
	template <typename _ValType>
		std::pair<typename radix_tree<_ValType>::iterator, bool>
		radix_tree<_ValType>::insert (index_t key, _ValType val)
		{
			std::pair<iterator, bool> retval;
			radix_node *parent, *slot, **slots;
			register u32 height, index, shift;

			while (key > max_index[rt_root->height])
			{
				rt_root = extend ();
			}

			parent = rt_root;
			slot = NULL;
			slots = (radix_node **)rt_root->slots;
			height = rt_root->height;
			index = 0;
			shift = (height) * RT_BRANCH_FACTOR_BIT;
			while (height > 0)
			{
				// retrieve array index in current level
				index = (key >> shift) & (RT_BRANCH_INDEX_MASK);
				slot = slots[index];
				if (slot == NULL)
				{	// extend slot
					slot = new radix_node ();
					slot->height = height - 1;
					slot->parent = parent;
					slot->offset = index;
					slots[index] = slot;
					parent->size++;
				}

				// move down a level
				slots = (radix_node **) slot->slots;
				parent = slot;
				shift -= RT_BRANCH_FACTOR_BIT;
				--height;
			}

			// ready to insert value after DFS
			index = key & (RT_BRANCH_INDEX_MASK);

			// insert entry
			parent->size++;

			// check for existence
			retval.second = slots[index] == NULL;
			if (retval.second)
			{
				retval.second = true;
				radix_node *p = new radix_node (val);
				p->parent = parent;
				slots[index] = p;
			}

			// insert to the iteration list
			list_head.insert (&slots[index]->node);
			slots[index]->node.container = parent;

			// return iterator with the destination item (no matter new/old)
			retval.first = iterator (&slots[index]->node);
			return retval;
		}

	/**
	 * find by key, returning iterator
	 */
	template <typename _ValType>
		typename radix_tree<_ValType>::iterator
		radix_tree<_ValType>::find (index_t key) const
		{
			const list_node *retval;
			radix_node *p, **slots;
			register u16 height, shift;
			register u32 index;

			if (key > max_index[rt_root->height])
				goto out_not_found;

			//slot = rt_root;
			slots = (radix_node **)rt_root->slots;
			height = rt_root->height;
			shift = (height) * RT_BRANCH_FACTOR_BIT;
			while (height > 0)
			{
				index = (key >> shift) & (RT_BRANCH_INDEX_MASK);
				if (slots[index] == NULL)
				{	// not found
					goto out_not_found;
				}

				//slot = slots[index];
				slots = (radix_node **) slots[index]->slots;
				shift -= RT_BRANCH_FACTOR_BIT;
				--height;
			}

			// arrived at substree height = 0
			index = (key >> shift) & (RT_BRANCH_INDEX_MASK);

			p = slots[index];
			if (p == NULL)
				goto out_not_found;
			retval = &slots[index]->node;

out:
			return iterator ((list_node *) retval);

out_not_found:
			retval = &list_tail;
			goto out;
		}

	/**
	 * erase by key
	 */
	template <typename _ValType>
		u32 radix_tree<_ValType>::erase (const index_t &key)
		{
			iterator it = find (key);
			if (it == end ())
				return 0;
			erase (it);
			return 1;
		}

	/**
	 * erase by iterator
	 */
	template <typename _ValType>
		void radix_tree<_ValType>::erase (const iterator &iter)
		{
			/*
			   char * p = (char*) iter.p;
			   p = p - offsetof (radix_node, node);
			   radix_node *r_node = (radix_node *) p;
			   */
			radix_node *r_node = (iter.p)->container;
//			radix_node *r_node = container_of (iter.p, radix_node, node);
			radix_node *p_node = r_node->parent;

			/**
			 * erase item in parent's slots
			 */
			p_node->slots [r_node->offset] = NULL;
			--p_node->size;
			if (p_node->size == 0)
				shrink (p_node);
			delete r_node;
		}
} // namespace li
#endif // __LITHIUM_RADIX_TREE_H__
