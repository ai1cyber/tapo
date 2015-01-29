#ifndef __CMD_OPTIONS_H__
#define __CMD_OPTIONS_H__

#include <stdint.h>

enum { Undetermined, Online, Offline };

extern int pcap_type;
extern char pcap_filename[1024];
extern char pcap_intf[128];
extern int pcap_limit;

extern char server_ip[128];
extern uint16_t server_port;

void parse_cmd_options(int argc, const char** argv);

#endif
