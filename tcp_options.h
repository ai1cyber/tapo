#ifndef __TCP_OPTIONS_H__
#define __TCP_OPTIONS_H__

#include <netinet/tcp.h>
#include <stdint.h>

#include "tcp_base.h"

#define TCP_OPT(tcphdr) ((char *)tcphdr + 20)
#define TCP_OPT_LEN(tcphdr) (tcphdr->doff*4 - 20)

#define IS_FLAG_SET(tcp_opt, opt_code) (tcp_opt->opt_flag & (1<<(opt_code)))
#define SET_FLAG(tcp_opt, opt_code) (tcp_opt->opt_flag = tcp_opt->opt_flag | (1<<(opt_code)))

// tcp options
#define TCPOPT_EOL 0 /* End of options */
#define TCPOPT_NOP 1 /* Padding */
#define TCPOPT_MSS 2 /* Segment size negotiating */
#define TCPOPT_WSCALE 3 /* Window scaling */
#define TCPOPT_SACK_PERM 4 /* SACK Permitted */
#define TCPOPT_SACK 5 /* SACK Block */
#define TCPOPT_TIMESTAMP 8 /* Better RTT estimations/PAWS */
#define TCPOPT_LIMIT 9

struct tcp_option {
	int opt_flag;
	uint16_t mss;
	uint8_t wscale;
	uint8_t sack_ok;
	struct sack_block sack;
	struct timeval ts;
};

int get_tcp_option(struct tcphdr *th, struct tcp_option *ptcpopt);

#endif
