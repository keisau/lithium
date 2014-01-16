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
	struct hlist_head
	{
		hlist_head *next, **pprev;
		hlist_head () : next (NULL), pprev (NULL) {}
		~hlist_head () {
			if (pprev) {
				hlist_head *node = container_of (pprev, hlist_head, next);
				node->next = next;
			}
			if (next)
				next->pprev = pprev;
		}
	};

	static inline void hlist_insert (hlist_head *head, hlist_head *item)
	{
		if (head && item)
		{
			item->next = head->next;
			if (item->next)
				item->next->pprev = &item->next;
			head->next = item;
			item->pprev = &head->next;
		}
	}

	static inline void hlist_delete (hlist_head *head)
	{
		if (head->pprev)
		{
			hlist_head *item = container_of (head->pprev, hlist_head, next);
			item->next = head->next;
			if (head->next) 
				head->next->pprev = &item->next;
		}
		head->next =NULL;
		head->pprev = NULL;
	}

} // namespace li

#endif // __LITHIUM_HLIST_H__
