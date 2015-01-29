#ifndef __TCP_PCAP_H__
#define __TCP_PCAP_H__

#include <pcap.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

pcap_t *pcap_init();
struct ip *get_ip_hdr(const u_char *pkt_ptr, int *cap_len);
void pcap_cleanup();

#endif
