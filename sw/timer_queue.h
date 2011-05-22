#ifndef _TIMER_QUEUE_H_
#define _TIMER_QUEUE_H_

#include <stdint.h>
#include "list.h"

struct timer_queue {
	struct list_head list;
	uint32_t expire;
	void (*func)(void *);
	void *data;
};

#define DECLARE_TIMER(name, func, data) \
	struct timer_queue name = __TIMER_INITIALIZER(func, data, 0)

#define __TIMER_INITIALIZER(_func, _data, _expire) \
	{ .func = (_func), .data = (_data), .expire = (_expire) }

void timer_del(struct timer_queue *timer);
void timer_mod(struct timer_queue *timer, uint32_t expires);
void timer_add(struct timer_queue *timer);
void timer_expire(uint32_t jiffies);

#endif
