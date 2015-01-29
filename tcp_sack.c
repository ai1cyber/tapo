#include "tcp_sack.h"
#include "tcp_range_list.h"
#include "malloc.h"
#include <assert.h>
#include <string.h>

#define SACK sack->block

int spurious_retrans(uint32_t snd_una, struct sack_block *sack, uint32_t *b, uint32_t *e)
{
	if (sack->num == 0)
		return 0;

	if (before(SACK[0].begin, snd_una)) {
		*b = SACK[0].begin;
		*e = MIN_SEQ(SACK[0].end, snd_una);
		return (*e - *b);
	}

	if (sack->num > 1) {
		if (!before(SACK[0].begin, SACK[1].begin) && \
				!after(SACK[0].end, SACK[1].end)) {
			*b = SACK[0].begin;
			*e = SACK[0].end;
			return (*e - *b);
		}
	}

	return 0;
}

uint32_t sacked(uint32_t snd_una, struct sack_block *sack)
{
	if (sack->num == 0)
		return 0;
	else {
		int i = 0;
		uint32_t sacked = 0;
		for (; i < sack->num; i++)
			sacked += SACK[i].end - SACK[i].begin;

		uint32_t t1, t2;
		return sacked - spurious_retrans(snd_una, sack, &t1, &t2);
	}
}

uint32_t max_sack_ack(struct sack_block *sack)
{
	uint32_t sack_ack = SACK[0].end;
	int i = 1;
	for (; i < sack->num; i++) {
		if (after(SACK[i].end, sack_ack))
			sack_ack = SACK[i].end;
	}

	return sack_ack;
}

void normalize(struct sack_block *sack)
{
	int i, j;
	int num = sack->num;
	if (num == 0)
		return ;

	// sort sack blocks
	for (i = num-1; i > 0; i--) {
		for (j = 0; j < i; j++) {
			if (after(SACK[j].begin, SACK[j+1].begin))
				swap(SACK[j], SACK[j+1]);
		}
	}

	// remove dup sacks
	int valid = num;
	for (i = 0, j = 1; i < num && j < num; i++, j++) {
		while (j < num) {
			if (!before(SACK[i].end, SACK[j].end)) {
				j += 1;
				valid -= 1;
			}
			else
				break;
		}
		if (j < num && i+1 < j) {
			// memcpy(&SACK[i+1], &SACK[j], sizeof(SACK[j]));
			SACK[i+1] = SACK[j];
		}
	}

	sack->num = valid;
}

/* calculate the number of bytes which are reordered. */
int get_reordering(uint32_t snd_una, struct sack_block *sack, 
		uint32_t *b, uint32_t *e) 
{
	if (sack->num == 0)
		return 0;
	else {
		int last = sack->num - 1;
		if (SACK[last].begin > snd_una) {
			*b = snd_una;
			*e = SACK[last].begin;
			return *e - *b;
		}
		else
			return 0;
	}
}

// sack has been normalized
void add_to_block_list(struct sack_block *sack, struct list_head *list)
{
	int i = 0;
	struct range_t *new;
	for (i = 0; i < sack->num; i++) {
		if (list_empty(list)) {
			new = MALLOC(struct range_t);
			new->begin = SACK[i].begin;
			new->end = SACK[i].end;
			list_insert(&new->list, list, list);
		}
		else {
			struct range_t *entry = list_entry(list->prev, struct range_t, list);
			if (!before(SACK[i].end, entry->end)) {
				if (after(SACK[i].begin, entry->end)) {
					new = MALLOC(struct range_t);
					new->begin = SACK[i].begin;
					new->end = SACK[i].end;
					list_insert(&new->list, list->prev, list);
				}
				else {
					entry->end = SACK[i].end;
				}
			}
		}
	}
}

#undef SACK
