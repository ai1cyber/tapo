#include "tcp_pcap.h"
#include "config.h"
#include "log.h"
#include "def.h"
#include "cmd_options.h"

#include <stdlib.h>
#include <errno.h>
#include <net/ethernet.h>

static int offset = 0;
static struct bpf_program fp;

pcap_t *pcap_init()
{
	pcap_t *handle;
	char errbuf[PCAP_ERRBUF_SIZE];
	if (pcap_type == Offline) {
		if(!(handle = pcap_open_offline(pcap_filename, errbuf))) {
			LOG(ERROR, "Could not open pcap file: %s\n", errbuf);
			exit(1);
		}
	}
	else {
		if (!(handle = pcap_open_live(pcap_intf, 96, 1, -1, errbuf))) {
			LOG(ERROR, "Could not open the device: %s\n", errbuf);
			exit(1);
		}
	}

	// set pcap filter
	char pf_buf[2048];
#define pf_fmt "tcp && ((src host %s && src port %d) || (dst host %s && dst port %d))"
	snprintf(pf_buf, sizeof(pf_buf), pf_fmt, server_ip, server_port, server_ip, server_port);
	if (pcap_compile(handle, &fp, pf_buf, 0, 0) == -1) {
	    LOG(ERROR, "Could not parse filter %s: %s.\n", pf_buf, pcap_geterr(handle));
	    exit(1);
	}
	
	if (pcap_setfilter(handle, &fp) == -1) {
	    LOG(ERROR, "Could not apply filter %s: %s.\n", pf_buf, pcap_geterr(handle));
	    exit(1);
	}

	// get pcap link type
	int link_type = pcap_datalink(handle);
	switch (link_type) {
		case LINKTYPE_ETHERNET:
			offset = 12;
			break;
		case LINKTYPE_LINUX_SLL:
			offset = 14;
			break;
		default:
			LOG(WARN, "Unknown link type (%x).\n", link_type);
			exit(1);
	}

	return handle;
}

struct ip *get_ip_hdr(const u_char *pkt_ptr, int *len)
{
	int ether_type = ntohs(*((uint16_t *)(pkt_ptr + offset)));
	if (ether_type == ETHERTYPE_IP) {
		pkt_ptr += (offset+2);
	}
	else {
		LOG(DEBUG, "Unknown packet type (%x).\n", ether_type);
		return NULL;
	}

	struct ip *ip_hdr = (struct ip *)pkt_ptr;
	int iphdr_len = ip_hdr->ip_hl * 4;
	*len -= (offset+2+iphdr_len);
	if (*len < 0)
		return NULL;
	else 
		return ip_hdr;
}

void pcap_cleanup(pcap_t *handle)
{
	pcap_freecode(&fp);
	pcap_close(handle);
}
