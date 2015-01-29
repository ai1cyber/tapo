#ifndef __TCP_BASE_H__
#define __TCP_BASE_H__

#include <netinet/in.h>

// including tcp state
#include <netinet/tcp.h>
// besides the standard states, we add an initial state
#define TCP_LISTEN 0

enum { DIR_UNDETERMINED = 0, DIR_IN, DIR_OUT };

struct tcp_key {
	struct in_addr addr[2];
	uint16_t port[2];
};

struct block_t {
	uint32_t begin;
	uint32_t end;
};

struct sack_block {
	struct block_t block[4];
	int num;
};

struct rtt_t {
	uint32_t srtt;
	uint32_t rttvar;
	uint32_t rto;
};

// 1 tick = 0.001 second
#define TIME_TO_TICK(t) ((int)((t)*1000))
#define TICK_TO_TIME(tick) ((double)(tick)/1000.0)
int rtt_thres(struct rtt_t *rtt);
void init_rtt(struct rtt_t *rtt);
void update_rtt(struct rtt_t *rtt, int32_t m);

static inline int before(uint32_t seq1, uint32_t seq2)
{
	return (int32_t)(seq1-seq2) < 0;
}
#define after(seq2, seq1) 	before(seq1, seq2)
#define MAX_SEQ(seq1, seq2) (before(seq1, seq2)?(seq2):(seq1))
#define MIN_SEQ(seq1, seq2) (before(seq1, seq2)?(seq1):(seq2))

// check whether seq1 is between seq2 (including) and seq3 (excluding)
// (seq2 < seq3)
static inline int between(uint32_t seq1, uint32_t seq2, uint32_t seq3)
{
	// return seq3 - seq2 >= seq1 - seq2;
	return seq3 - seq2 > seq1 - seq2;
}

#define BYTES_TO_PKTS(bytes, segsize) (bytes/seg_size+((bytes%seg_size)==0?0:1))

#define swap(a, b) \
	do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)

#endif
