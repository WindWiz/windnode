#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "list.h"

struct dummy {
	struct list_head list;	
	int val;
};

static bool test_add_tail(int n)
{
	int i;
	struct list_head mylist;
	struct list_head *tmp;
	
	list_init(&mylist);
	for(i = 0; i < n; i++) {
		struct dummy *item = (struct dummy *) malloc(sizeof(struct dummy));
		list_add_tail(&mylist, &item->list);
	}

	i = 0;
	list_for_each(tmp, &mylist) {
		i++;
	}

	if (i != n)
		printf("%s: %d != %d\n", __func__, i, n);

	return i == n;
}

static bool test_entry(void)
{
	struct list_head mylist;
	struct list_head *tmp;
	struct dummy a;
		
	list_init(&mylist);
	
	a.val = 9;
	list_add(&mylist, &a.list);
	
	list_for_each(tmp, &mylist) {
		struct dummy *entry = list_entry(tmp, struct dummy, list);
	
		if (entry != &a) {
			printf("%s: entry is not what was added!\n", __func__);
			return false;
		}

		if (entry->val != 9) {
			printf("%s: entry value (%d) is corrupt\n", __func__, entry->val);
			return false;
		}
	}

	return true;
}

static bool test_empty(void)
{
	struct list_head empty;
	list_init(&empty);

	if (!list_empty(&empty)) {
		printf("%s: list not empty!\n", __func__);
		return false;
	} else
		return true;
}

static bool test_del(void)
{
	struct list_head list;
	struct list_head *tmp;
	struct dummy d[3];
	int i;

	list_init(&list);
	list_add_tail(&list, &d[0].list);
	list_add_tail(&list, &d[1].list);
	list_add_tail(&list, &d[2].list);

	i = 0;
	list_for_each(tmp, &list) {
		if (&d[i].list != tmp) {
			printf("%s: unexpected order before del\n", __func__);
			return EXIT_FAILURE;
		}

		i++;
	}
	
	list_del(&d[1].list);

	i = 0;
	list_for_each(tmp, &list) {
		if (&d[i].list != tmp) {
			printf("%s: unexpected order after first del\n", __func__);
			return EXIT_FAILURE;
		}

		i += 2;
	}

	list_del(&d[0].list);

	if (list_first_entry(&list, struct dummy, list) == &d[2])
		return true;
	else {
		printf("%s: unexpected last item after both del\n", __func__);
		return false;
	}
}

int main(void)
{	
	if (!test_add_tail(100))
		return EXIT_FAILURE;	

	if (!test_empty())
		return EXIT_FAILURE;

	if (!test_entry())
		return EXIT_FAILURE;

	if (!test_del())
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

