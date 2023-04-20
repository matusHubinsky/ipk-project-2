
#ifndef SCANNER_H_
#define SCANNER_H_

#include "arg.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include <netdb.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/ip.h>	
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <netpacket/packet.h>

#include <netinet/ip6.h>
#include <netinet/icmp6.h>


extern void udp_scan(struct arguments_t *arguments);

#endif 