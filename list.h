#ifndef __TCP_LIST_H__
#define __TCP_LIST_H__

#include "malloc.h"

struct list_head {
	struct list_head *next, *prev;
};

#define list_empty(list) ((list)->next == (list))

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member))) 

static inline void init_list_head(struct list_head *list)
{
	list->next = list->prev = list;
}

static inline void list_insert(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new;
	prev->next = new;
	new->next = next;
	new->prev = prev;
}

static inline void list_delete_entry(struct list_head *this)
{
	this->next->prev = this->prev;
	this->prev->next = this->next;
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	list_insert(new, head->prev, head);
}

#define delete_list(list, type, member) \
do { \
	struct list_head *p = (list)->next, \
					 *t = NULL; \
	while (p != (list)) { \
		t = p; \
		p = p->next; \
		FREE(list_entry(t, type, member)); \
		list_delete_entry(t); \
	} \
} while (0)

#endif
