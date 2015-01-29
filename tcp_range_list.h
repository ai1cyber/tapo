#ifndef __TCP_RANGE_LIST_H__
#define __TCP_RANGE_LIST_H__

#include <stdint.h>
#include "list.h"

struct range_t {
	uint32_t begin;
	uint32_t end;
	struct list_head list;
};

int in_range_list(uint32_t n, struct list_head *list);
void append_to_range_list(struct list_head *list, uint32_t begin, uint32_t end);
uint32_t list_size(struct list_head *list);
uint32_t list_range_size(struct list_head *list, uint32_t b, uint32_t e);
void delete_range_list(struct list_head *list);

#endif
