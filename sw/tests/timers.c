#include <stdlib.h>
#include <stdio.h>

#include "timer_queue.h"

static void timer_done(void *p);

static struct timer_queue t0, t1;
static int t0_hit, t1_hit;
static uint32_t jiffies;

static void timer_done(void *p)
{
	struct timer_queue *x = (struct timer_queue *)p;

	if (x == &t0) {
		timer_mod(x, jiffies + 50);
		t0_hit++;
	} else if (x == &t1) {
		timer_mod(x, jiffies + 100);
		t1_hit++;
	} else {
		printf("timers: %s: unknown timer hit (%p)\n", __func__, p);
		return;
	}
}

static int test_no_expire(void)
{
	t0.func = t1.func = timer_done;
	t0.data = &t0;
	t1.data = &t1;

	t0_hit = t1_hit = 0;
	jiffies = 0;
	t0.expire = 50;
	timer_add(&t0);
	t1.expire = 100;
	timer_add(&t1);

	timer_expire(jiffies);

	if (t0_hit)
		printf("timers: %s: t0 expired prematurely\n", __func__);
	if (t1_hit)
		printf("timers: %s: t1 expired prematurely\n", __func__);

	timer_del(&t0);
	timer_del(&t1);

	return !(t0_hit || t1_hit);
}

static int test_t0_expire(void)
{
	t0.func = t1.func = timer_done;
	t0.data = &t0;
	t1.data = &t1;

	t0_hit = t1_hit = 0;
	jiffies = 50;
	t0.expire = 50;
	timer_add(&t0);
	t1.expire = 100;
	timer_add(&t1);

	timer_expire(jiffies);

	if (!t0_hit)
		printf("timers: %s: t0 did not expire as expected\n", __func__);
	if (t1_hit)
		printf("timers: %s: t1 expired prematurely\n", __func__);

	timer_del(&t0);
	timer_del(&t1);

	return !(!t0_hit && t1_hit);
}

static int test_t0_t1_expire(void)
{
	t0.func = t1.func = timer_done;
	t0.data = &t0;
	t1.data = &t1;

	t0_hit = t1_hit = 0;
	jiffies = 100;
	t0.expire = 50;
	timer_add(&t0);
	t1.expire = 100;
	timer_add(&t1);

	timer_expire(jiffies);

	if (!t0_hit)
		printf("timers: %s: t0 did not expire as expected\n", __func__);
	if (!t1_hit)
		printf("timers: %s: t1 did not expire as expected\n", __func__);

	timer_del(&t0);
	timer_del(&t1);

	return !(t0_hit && t1_hit);
}

static int test_periodic_expire(void)
{
	t0.func = timer_done;
	t0.data = &t0;

	t0_hit = 0;
	jiffies = 0;
	t0.expire = 50;
	timer_add(&t0);

	while (jiffies < 200) {
		jiffies += 25;
		timer_expire(jiffies);
	}

	if (t0_hit != 4) {
		printf("timers: %s: timer expired %d times (unexpected)\n", __func__,
		    t0_hit);
	}

	return (t0_hit == 4);
}

int main(void)
{
	if (!test_no_expire())
		return EXIT_FAILURE;

	if (!test_t0_expire())
		return EXIT_FAILURE;

	if (!test_t0_t1_expire())
		return EXIT_FAILURE;

	if (!test_periodic_expire())
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
