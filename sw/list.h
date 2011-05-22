#ifndef _LIST_H_
#define _LIST_H_

/* Linux kernel doubly linked list implementation */

#include <stdint.h>
#include "target.h"

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#define DECLARE_LIST(name) \
	struct list_head name = __LIST_INITIALIZER(&name)

#define __LIST_INITIALIZER(lst) \
	{ .next = (lst), .prev = (lst) }

static inline void __list_add(struct list_head *n, struct list_head *prev,
	struct list_head *next)
{
	next->prev = n;
	n->next = next;
	n->prev = prev;
	prev->next = n;
}

static inline void __list_del(struct list_head *prev,
	struct list_head *next)
{
	prev->next = next;
	next->prev = prev;
}

static inline void list_del(struct list_head *list)
{
	__list_del(list->prev, list->next);
}

static inline void list_add(struct list_head *head, struct list_head *n)
{
	__list_add(n, head, head->next);
}

static inline void list_add_tail(struct list_head *head, struct list_head *n)
{
	__list_add(n, head->prev, head);
}

static inline void list_init(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline uint8_t list_empty(struct list_head *head)
{
	return head->next == head;
}

#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

#endif /* _LIST_H_ */
