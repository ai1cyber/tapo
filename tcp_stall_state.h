#ifndef __TCP_STALL_STATE_H__
#define __TCP_STALL_STATE_H__

#include "tcp_base.h"
#include "list.h"
#include <stdio.h>

struct tcp_state;

struct tcp_stall_state {
	int init_rwnd;
	int max_snd_seg_size;

	int rwnd;
	int ca_state;

	double cur_time;
	double duration;
	double srtt;
	double rto;

	uint32_t snd_una;
	uint32_t snd_nxt;

	int packets_out;
	int sacked_out;
	int holes;
	int outstanding;
	int lost;
	int spurious;

	int cur_pkt_dir;
	int cur_pkt_lost;
	int cur_pkt_spurious;

	int head;
	int tail;

	struct list_head list;
};

void init_tcp_stall(struct tcp_state *ts, struct tcp_stall_state *tss, double duration);
void fill_tcp_stall_list(struct tcp_state *ts, struct list_head *stall_list);
void dump_tss_info(FILE *fp, struct tcp_stall_state *tss);

#endif
