#include "tcp_rtt.h"
#include "tcp_base.h"
#include "malloc.h"

void insert_seq_rtt(uint32_t ack_seq, double t, struct list_head *list)
{
	struct seq_rtt_t *new = MALLOC(struct seq_rtt_t);
	new->ack_seq = ack_seq;
	new->time = t;
	// just append the node to list tail
	list_add_tail(&new->list, list);
}

static void delete_rtt_list_prev(struct list_head *p, struct list_head *list)
{
	struct list_head *t = NULL;
	while (p != list) {
		t = p;
		p = p->prev;
		list_delete_entry(t);
		FREE(list_entry(t, struct seq_rtt_t, list));
	}
}

int get_rtt(uint32_t ack, double t, struct list_head *list)
{
	struct list_head *pos;
	struct seq_rtt_t *node;
	int found = 0;
	// search reversely
	list_for_each_prev(pos, list) {
		node = list_entry(pos, struct seq_rtt_t, list);
		if (ack == node->ack_seq) {
			found = 1;
			break;
		}
		else if (after(ack, node->ack_seq))
			break;
	}

	int rtt = 0;
	if (found == 1) {
		rtt = TIME_TO_TICK(t - node->time);
		delete_rtt_list_prev(pos, list);
	}

	return rtt;
}

void delete_rtt_list(struct list_head *list)
{
	delete_rtt_list_prev(list->prev, list);
}
