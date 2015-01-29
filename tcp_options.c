#include "tcp_options.h"
#include "log.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int get_tcp_option(struct tcphdr *th, struct tcp_option *ptcp_opt)
{
	memset(ptcp_opt, 0, sizeof(struct tcp_option));

	char *options = TCP_OPT(th);
	int len = TCP_OPT_LEN(th);
	if (len <= 0) 
		return 0;

	int opt_code = 0;
	int opt_len = 0;
	int itr = 0;
	while (itr < len) {
		opt_code = (int)(options[itr]);

		if (opt_code == TCPOPT_EOL)
			return 0;
		else if (opt_code == TCPOPT_NOP) {
			itr++;
			continue;
		}

	   	if (IS_FLAG_SET(ptcp_opt,opt_code)) {
	   		LOG(DEBUG, "opt_code[%d] already set\n", opt_code);
	   		break;
	   	}	
	   	opt_len = (int) (options[itr+1]);
	   	opt_len -= 2;
	
	   	if (opt_len < 0) { 
	   		LOG(ERROR, "tcp option is invalid :%d %d \n", opt_code, opt_len);		
	   		return -1;
	   	}

	   	switch (opt_code) {
	   		case TCPOPT_MSS:
	   			ptcp_opt->mss = ntohs(*(uint16_t*)(options+itr+2));
	   			break;

	   		case TCPOPT_WSCALE:
	   			ptcp_opt->wscale = *(uint8_t *)(options+itr+2);
	   			break;

	   		case TCPOPT_SACK_PERM:
	   			ptcp_opt->sack_ok = 1;
	   			break;	

	   		case TCPOPT_SACK:
	   			if (opt_len % 8 != 0) {
	   				LOG(ERROR, "sack option is invalid, length = %d.\n", opt_len);
	   				return -1;
	   			}		
	   			ptcp_opt->sack.num = opt_len / 8;
	   			int i = 0;
	   			for (i = 0 ; i < opt_len / 8; i++){
	   				ptcp_opt->sack.block[i].begin = ntohl(*(uint32_t *)(options+itr+2+i*8));
	   				ptcp_opt->sack.block[i].end = ntohl(*(uint32_t *)(options+itr+2+i*8+4));
	   			}
	   			break;

	   		case TCPOPT_TIMESTAMP:
	   			if (opt_len % 4 != 0) {
	   				LOG(ERROR, "timestamp option is invalid, length = %d.\n", opt_len);
	   				return -1;
	   			}
	   			ptcp_opt->ts.tv_sec = ntohl(*(uint32_t *)(options+itr+2));
	   			ptcp_opt->ts.tv_usec = ntohl(*(uint32_t *)(options+itr+2+4));	
	   			break;

	   		default:
	   			LOG(DEBUG, "unknown option %d\n", opt_code);
	   			break;
	   	}

	   	if (opt_code < TCPOPT_LIMIT)
	   		SET_FLAG(ptcp_opt, opt_code);

	   	itr += opt_len+2;
	}

	return 0;
}
