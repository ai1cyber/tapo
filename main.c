#include "def.h"
#include "config.h"
#include "tcp_state.h"
#include "tcp_pcap.h"
#include "hash_table.h"
#include "malloc.h"
#include "log.h"
#include "cmd_options.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pcap.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

struct in_addr server;
pcap_t *pcap_handle;
struct hash_table_entry **hash_table;
static int pkt_counter = 0;

void cleanup();
static void handle_signal(int signo)
{
	fprintf(stdout, "catch signo %d, finishing...\n", signo);
	cleanup();
	fprintf(stdout, "finished.\n");

	exit(0);
}

static void register_signal()
{
	if (signal(SIGINT, &handle_signal) == SIG_ERR) {  
		LOG(ERROR, "Couldn't register signal hanlder!\n");  
		exit(1);  
	}

	if (signal(SIGTERM, &handle_signal) == SIG_ERR) {  
		LOG(ERROR, "Couldn't register signal hanlder!\n");  
		exit(1);  
	}
}

void init()
{
	pcap_handle = pcap_init();

	if (inet_aton(server_ip, &server) != 1) {
		LOG(ERROR, "Could not convert server ip to in_addr\n");
		exit(1);
	}

	register_signal();

	hash_table = new_hash_table();
}

void cleanup()
{
	pcap_cleanup(pcap_handle);
	cleanup_hash_table(hash_table);
}

static void parse_tcp_info(struct tcp_key *key, double time, struct tcphdr *th, int len, int dir)
{
	// LOG(INFO, "time: %.6lf, len: %d, dir: %d\n", time, len, dir);
	struct tcp_state * ts = find_ts_entry(hash_table, key);
	if (ts == NULL && IS_SYN(th) && dir == DIR_IN) {
		ts = new_tcp_state(key, time);
		insert_ts_entry(hash_table, ts);
	}

	if (ts != NULL) {
		tcp_state_machine(ts, th, len, time, dir);

		if (ts->state == TCP_CLOSE || ts->state == TCP_CLOSING) {
			delete_ts_entry(hash_table, ts);
			// free_tcp_state(ts);
		}
	}
}

void handle_pcap()
{
	struct pcap_pkthdr pph;
	const u_char *packet;
	while ((packet = pcap_next(pcap_handle, &pph))) {
		pkt_counter += 1;
		if (pcap_limit > 0 && pkt_counter >= pcap_limit) {
			LOG(INFO, "finished...\n");
			exit(0);
		}

		double time = (double)pph.ts.tv_sec + (double)(pph.ts.tv_usec)/1000000;

		int len = pph.caplen;
		struct ip *ip_hdr = get_ip_hdr(packet, &len);
		if (ip_hdr == NULL)
			continue;

		struct tcphdr *tcp_hdr = (struct tcphdr *)((u_char *)ip_hdr + ip_hdr->ip_hl*4);
		int iphdr_len = ip_hdr->ip_hl*4;
		int tcphdr_len = tcp_hdr->doff*4;

		if (tcphdr_len > len) {
			LOG(DEBUG, "tcp header is not captured completely.\n"); 
			continue;
		}

		/* get tcp key */
		int dir;
		struct tcp_key key;
		if (memcmp(&ip_hdr->ip_src, &server, sizeof(server)) == 0) {
			key.addr[0] = ip_hdr->ip_src;
			key.addr[1] = ip_hdr->ip_dst;
			key.port[0] = tcp_hdr->source;
			key.port[1] = tcp_hdr->dest;
			dir = DIR_OUT;
		}
		else {
			key.addr[0] = ip_hdr->ip_dst;
			key.addr[1] = ip_hdr->ip_src;
			key.port[0] = tcp_hdr->dest;
			key.port[1] = tcp_hdr->source;
			dir = DIR_IN;
		}

		int payload_len = ntohs(ip_hdr->ip_len) - iphdr_len - tcphdr_len;

		/* parse tcp info */
		parse_tcp_info(&key, time, tcp_hdr, payload_len, dir);
	}
}

int main(int argc, const char **argv)
{
	parse_cmd_options(argc, argv);

	init();
	handle_pcap();
	cleanup();

	return 0;
}
