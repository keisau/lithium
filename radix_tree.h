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
 * Radix tree implementation based on linux kernel's radix tree.
 *
 * Author: Pierre Saux
 * First commit date: 3-Jan-2014
 *
 * Changelog:
 * pierresaux (16-jan-14):Refactor, reuse list_head and list_head_iterator
 *
 * You may find the original version in:
 * 
 * Header: $(linux_src)/include/linux/radix_tree.h
 * Source: $(linux_src)/lib/radix_tree.c
 */
#ifndef __LITHIUM_RADIX_TREE_H__
#define __LITHIUM_RADIX_TREE_H__

#include "lithium.h"
#include "list.h"
#include "iterator.h"
#include <stdio.h>
namespace li
{
	/**
	 * Branch factor must be a power of 2. Default to 2^6 (copied from kernel)
	 */
#define RT_BRANCH_FACTOR_BIT	(6)
#define RT_BRANCH_FACTOR		(1 << RT_BRANCH_FACTOR_BIT)
#define RT_BRANCH_INDEX_MASK	(RT_BRANCH_FACTOR - 1)
#define RT_INDIRECT_NODE		0
#define RT_DATA_NODE			1

//	template <typename _ValType>


	/**
	 * radix_tree container
	 */
	template <typename _ValType>
		class radix_tree
		{
		protected:
	/**
	 * radix tree node
	 */
			struct radix_node
			{
				_ValType		value;
				radix_node		*parent;	// parent slot
				u16				height;		// height of subtree
				u16				size;		// number of occupied slots
				u16				offset;		// parent->slots[offset] == this
				u16				flags;

				// linked list for O(n) stepwise iteration (BFS)
				list_head		head;

				radix_node		*slots [RT_BRANCH_FACTOR];

				radix_node () :	parent (this),
					height (0),
					size (0),
					offset (0),
					flags (RT_INDIRECT_NODE),
					slots () {}

				radix_node (_ValType value) : flags (RT_DATA_NODE),
					value (value) {}

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
			// types
		public:
			typedef li::list_head_iterator <radix_node, _ValType> iterator;
			friend class li::list_head_iterator <radix_node, _ValType>;

			// functions
		public:
			radix_tree ()
			{
				rt_root = new radix_node ();
				list_insert (&head, &tail);

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
				radix_node *node = rt_root;
				if (node)
					delete node;
			}

			iterator begin () { return iterator (head.next); }
			iterator end () { return iterator (&tail); }

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
			list_head		head;		// begin()
			list_head		tail;		// end()
			u32				max_height;
			index_t			max_index[sizeof(index_t) << 3 /* size of a byte */];
		};

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
				radix_node *p = new radix_node (val);
				p->parent = parent;
				slots[index] = p;
			}

			// insert to the iteration list
			list_insert (&head, &slots[index]->head);
			//slots[index]->head.container = parent;

			// return iterator with the destination item (no matter new/old)
			retval.first = iterator (&slots[index]->head);
			return retval;
		}

	/**
	 * find by key, returning iterator
	 */
	template <typename _ValType>
		typename radix_tree<_ValType>::iterator
		radix_tree<_ValType>::find (index_t key) const
		{
			const list_head *retval;
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
				p = slots[index];
				if (p == NULL)
				{	// not found
					goto out_not_found;
				}

				slots = (radix_node **) slots[index]->slots;
				shift -= RT_BRANCH_FACTOR_BIT;
				--height;
			}

			// arrived at substree height = 0, found
			retval = &p->head;
out:
			return iterator ((list_head *) retval);

out_not_found:
			retval = &tail;
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
			list_head * tmp = iter.p;
			radix_node *r_node;
#ifndef _WIN32
			r_node = container_of (tmp, radix_node, head);
#else
			char * _p = (char*) iter.p;
			r_node = (radix_node*)(_p - offsetof (radix_node, head));
#endif

			radix_node *p_node = r_node->parent;

			/**
			* erase item in parent's slots
			*/
			printf ("DEBUG : %d\n", r_node->offset);
			p_node->slots [r_node->offset] = NULL;
			--p_node->size;
			if (p_node->size == 0)
				shrink (p_node);
			delete r_node;
		}
} // namespace li
#endif // __LITHIUM_RADIX_TREE_H__
