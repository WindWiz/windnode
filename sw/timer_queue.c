#include <stdlib.h>
#include <stdint.h>

#include "list.h"
#include "timer_queue.h"

static DECLARE_LIST(timer_list);

static int timer_pending(const struct timer_queue *timer)
{
	return timer->list.next != NULL;
}

void timer_del(struct timer_queue *timer)
{
	if (timer_pending(timer))
		list_del(&timer->list);
}

void timer_mod(struct timer_queue *timer, uint32_t expire)
{
	struct list_head *tmp;

	timer_del(timer);

	timer->expire = expire;

	if (list_empty(&timer_list)) {
		list_add_tail(&timer_list, &timer->list);
		return;
	}

	/* find insertion point in sorted list */
	list_for_each(tmp, &timer_list) {
		struct timer_queue *p = list_entry(tmp, struct timer_queue, list);

		if (timer->expire < p->expire)
			break;
	}

	list_add_tail(tmp, &timer->list);
	return;
}

void timer_add(struct timer_queue *timer)
{
	timer_mod(timer, timer->expire);
}

void timer_expire(uint32_t jiffies)
{
	struct list_head *cur;
	struct list_head *tmp;

	list_for_each_safe(cur, tmp, &timer_list) {
		struct timer_queue *timer =
		    list_entry(cur, struct timer_queue, list);

		if (timer->expire <= jiffies) {
			list_del(&timer->list);
			timer->func(timer->data);
		}
	}
}
