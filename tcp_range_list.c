#include "tcp_range_list.h"
#include "tcp_base.h"
#include "malloc.h"

#include <stdlib.h>

// void append_to_range_list(struct list_head *list, struct range_t *range)
// {
// 	struct range_t *r = (struct range_t *)MALLOC(sizeof(struct range_t));
// 	memcpy(r, range, sizeof(struct range_t));
// 	list_add_tail(&r->list, list);
// }

void append_to_range_list(struct list_head *list, uint32_t begin, uint32_t end)
{
	struct range_t *range = MALLOC(struct range_t);
	range->begin = begin;
	range->end = end;
	list_add_tail(&range->list, list);
}

uint32_t list_size(struct list_head *list)
{
	struct list_head *p;
	uint32_t size = 0;
	list_for_each(p, list) {
		struct range_t *node = list_entry(p, struct range_t, list);
		size += (node->end - node->begin);
	}

	return size;
}

uint32_t list_range_size(struct list_head *list, uint32_t b, uint32_t e)
{
	struct list_head *p;
	uint32_t size = 0;
	list_for_each(p, list) {
		struct range_t *node = list_entry(p, struct range_t, list);
		if (!before(node->begin, e) || !before(b, node->end))
			continue;
		else
			size += (MIN_SEQ(node->end, e) - MAX_SEQ(node->begin, b));
	}

	return size;
}

int in_range_list(uint32_t n, struct list_head *list)
{
	struct list_head *pos;
	list_for_each_prev(pos, list) {
		struct range_t *r = list_entry(pos, struct range_t, list);
		if (between(n, r->begin, r->end))
			return 1;
		else if (before(n, r->begin))
			return 0;
	}

	return 0;
}

void delete_range_list(struct list_head *list)
{
	delete_list(list, struct range_t, list);
}
