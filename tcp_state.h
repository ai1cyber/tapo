#ifndef __TCP_STATE_H__
#define __TCP_STATE_H__

#include "tcp_base.h"
#include "tcp_options.h"
#include "tcp_range_list.h"

#include "def.h"

#include <stdio.h>

#define IS_SYN(th) th->syn
#define IS_RST(th) th->rst
#define IS_FIN(th) th->fin
#define IS_ACK(th) !(th->syn || th->rst || th->fin)

#define TCP_CA_OPEN 0
#define TCP_CA_RECOVERY 1
extern const char *tcp_ca_state[];

struct tcp_state {
	struct tcp_key key;
	char name[128];

	int state; // see /usr/include/netinet/tcp.h
	uint32_t snd_nxt;
	uint32_t snd_una;
	uint32_t rcv_nxt;
	uint32_t rcv_una;

	uint32_t seq_base;
	uint32_t max_snd_seg_size;

	int pkt_cnt; // n-th pkt in the flow, for debugging

	struct tcp_option option;
	// in options
	struct sack_block sack;
	int rwnd_scale;
	int rwnd;
	int init_rwnd;

	// TCP_CA_OPEN, TCP_CA_RECOVERY
	int ca_state;

	// time related
	double start_time;
	double last_time;
	double last_in_time;
	double last_out_time;

	int last_pkt_dir; // In, Out, Undetermined

	struct rtt_t rtt;

	// retransmit
	uint32_t recovery_point;

	double last_stall_time;
	uint32_t last_stall_point;
	uint32_t stall_cnt;

	struct block_t reord;

	struct list_head rtt_list;
	struct list_head block_list;
	struct list_head retrans_list;
	struct list_head reordering_list;
	struct list_head spurious_retrans_list;
	struct list_head lost_list;

	int stall_dir_waiting;
	struct list_head stall_list;

	int packets_out;
	int fackets_out;
	int sacked_out;
	int holes;
	int retrans_out;
	int outstanding;

	// indicate whether it's at the beginning/end of a transferring file
	int head;
	int tail;
};

struct tcp_state *new_tcp_state(struct tcp_key *key, double time);
int tcp_state_machine(struct tcp_state *ts, struct tcphdr *th, int len, double cap_time, int dir);
void finish_tcp_state(struct tcp_state *ts);
void dump_ts_info(FILE *fp, struct tcp_state *ts);

#endif
