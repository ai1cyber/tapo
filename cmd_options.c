#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h> 
#include <string.h>

#include "def.h"

#include "cmd_options.h"

int pcap_type = Undetermined;
char pcap_filename[1024] = { 0 };
char pcap_intf[128] = { 0 };
int pcap_limit = 0;

char server_ip[128] = { 0 };
uint16_t server_port;

#define _(x) x

const char *usage = 
	"Usage:\n"
	"    " PROG_NAME " [ -f pcap_file | -i pcap_intf ] -s server_ip -p server_port { -c count }\n"
	"\n"
	"Examples:\n"
	"    " PROG_NAME " -f file.pcap -s 10.21.0.202 -p 80\n"
	"    " PROG_NAME " -i eth0 -s 10.21.0.202 -p 80 -c 10000\n";

static void print_version()
{
	fprintf(stdout, "%s: %s\n\n", PROG_NAME, PROG_VERSION);
}

static void usage_exit(int status)
{
	fprintf(stderr, "%s", usage);
	exit(status);
}

void parse_cmd_options(int argc, const char **argv)
{
	int sflag = 0;
	int pflag = 0;
	const char* options = "hvf:i:s:p:c:";
	char cmd_opt;

	while ((cmd_opt = getopt(argc, (char **)argv, options)) != -1) {
		switch (cmd_opt) {	
			case 'f':
				if (pcap_type == Online)
					usage_exit(1);

				strncpy(pcap_filename, optarg, sizeof(pcap_filename)-1);
				pcap_type = Offline;
				break;

			case 'i':
				if (pcap_type == Offline)
					usage_exit(1);

				strncpy(pcap_intf, optarg, sizeof(pcap_intf)-1);
				pcap_type = Online;
				break;

			case 's':
				strncpy(server_ip, optarg, sizeof(server_ip)-1);
				sflag = 1;
				break;

			case 'p':
				if (sscanf(optarg, "%hu", &server_port) != 1)
					usage_exit(1);
				pflag = 1;
				break;

			case 'c':
				if (sscanf(optarg, "%d", &pcap_limit) != 1)
					usage_exit(1);
				break;

			case 'v':
				print_version();
				usage_exit(0);
				break;

			case 'h':
				usage_exit(0);
				break;

			case '?':
			default:
				usage_exit(1);
				break;
		}
	}

	if (pcap_type == Undetermined || !sflag || !pflag)
		usage_exit(1);
}
