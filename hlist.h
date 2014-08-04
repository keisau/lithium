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
* hlist implementation based on linux kernel's hlist_node & hlist_node.
*
* Author: Pierre Saux
* First commit date: 6-Jan-2014
*
* You may find the original version in:
* 
* Type definition: $(linux_src)/include/linux/lithium/types.h
* hlist operation: $(linux_src)/include/linux/hlist.h
*/
#ifndef __LITHIUM_HLIST_H__
#define __LITHIUM_HLIST_H__

#include <lithium.h>
#include <lithium/types.h>
#include <list.h>
#include <lithium/iterator.h>

namespace li
{
struct hlist_node
{
	hlist_node *next, **pprev;
	hlist_node () : next (NULL), pprev (NULL) {}
	~hlist_node () {
		/*
		if (pprev) {
			hlist_node *node = container_of (pprev, hlist_node, next);
			node->next = next;
		}
		if (next)
			next->pprev = pprev;
			*/
	}
};

struct hlist_head
{
	hlist_node *head;
	hlist_head () : head (NULL) {}
	~hlist_head () {}

	void insert (hlist_node *node) {
		hlist_node *first = head;
		node->next = head;
		if (first)
			first->pprev = &node->next;

		head = node;
		node->pprev = &head;
	}
};

static inline void hlist_push_back (hlist_node *head, hlist_node *item)
{
	head->pprev = item->pprev;
	head->next = item;
	item->pprev = &head->next;
	*(item->pprev) = item;
}

static inline void hlist_push_front (hlist_node *head, hlist_node *item)
{
	item->next = head->next;
	if (item->next)
		item->next->pprev = &item->next;
	head->next = item;
	item->pprev = &head->next;
}

static inline void hlist_delete (hlist_node *head)
{
	if (head) {
		if (head->pprev) {
			hlist_node *item = container_of (head->pprev, hlist_node, next);
			item->next = head->next;
			if (head->next) 
				head->next->pprev = &item->next;
		}
		head->next =NULL;
		head->pprev = NULL;
	}
}
} // namespace li

#endif // __LITHIUM_HLIST_H__
