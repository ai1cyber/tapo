#ifndef __TCP_SACK_H__
#define __TCP_SACK_H__

#include "tcp_base.h"
#include "list.h"

uint32_t sacked(uint32_t snd_una, struct sack_block *sack);
uint32_t max_sack_ack(struct sack_block *sack);
int spurious_retrans(uint32_t snd_una, struct sack_block *sack, uint32_t *b, uint32_t *e);
void normalize(struct sack_block *sack);
int get_reordering(uint32_t snd_una, struct sack_block *sack, uint32_t *b , uint32_t *e);
void add_to_block_list(struct sack_block *sack, struct list_head *list);

#endif
