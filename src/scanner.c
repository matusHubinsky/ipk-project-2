
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
#include <netinet/if_ether.h>
#include <netpacket/packet.h>

#include "../inc/scanner.h"

#define MAXPORT 65535   		// maximum port to scan
#define MAX_PACKET_SIZE 1024	

// TODO: detect diference between IPv4 and IPv6

struct thread_arg_t {
	char *interface; 
	char *ip_adress;
	int port;
};

int client_socket;
int range = 0;
int packet_num = 0;

// put all nubmers separeted with ','  in arr
// in case of "-" put all number is arr
static int* calculate_port_number(char port_range[], int arr[]) {    
	if (strchr(port_range, ',') != NULL) {
		int i = 0;
		char* token;
		while ((token = strsep(&port_range, ",")) != NULL) {
			arr[i] = atoi(token);
			i++;
		}
		range = i;
	}
	else if (strchr(port_range, '-') != NULL) {
		char *search = "-";
		int from = atoi(strtok(port_range, search));
		int to = atoi(strtok(NULL, search));
		// fill array
		for (int i = 0; i < (to - from); i++) {
			arr[i] = i + from;
			range++;
		}
	}

	return arr;
}


unsigned short csum(unsigned short *ptr, int nbytes) {
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }

    if (nbytes == 1) {
        oddbyte = 0;
        *((unsigned char *) &oddbyte) = *(unsigned char *) ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short) ~sum;

    return answer;
}


char* get_local_saddr() {
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *saddr = NULL;
    char buf[256];
    
    if (getifaddrs(&ifap) == -1) {
        perror("getifaddrs");
        return NULL;
    }

    // Loop through the list of interfaces
    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }
        
        if (ifa->ifa_addr->sa_family == AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
			// TODO: podpora IPv6
            inet_ntop(AF_INET, &(sa->sin_addr), buf, 256);
            
            // Ignore loopback interface
            if (strcmp(ifa->ifa_name, "lo") != 0) {
                saddr = strdup(buf);
                break;
            }
        }
    }
    
    freeifaddrs(ifap);
    return saddr;
}


void* sent_TCP(void* args) {
	int sockfd, one = 1, if_index;
	struct thread_arg_t* func_thread_arg = args;
	char* interface = func_thread_arg->interface;

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (sockfd < 0) {
		perror("socket");
		exit(1);
	}

	if (interface != NULL) {
		setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, interface, strlen(interface));
	}

	// Set IP_HDRINCL option to include IP header in the packet
	if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
		perror("setsockopt");
		close(sockfd);
		exit(1);
	}

	struct sockaddr_in dst_addr;
	memset(&dst_addr, 0, sizeof(dst_addr));
	dst_addr.sin_family = AF_INET;
	dst_addr.sin_addr.s_addr = inet_addr(func_thread_arg->ip_adress);
	dst_addr.sin_port = htons(func_thread_arg->port);

	fprintf(stderr, "sending: %s:%d\n", inet_ntoa(dst_addr.sin_addr), ntohs(dst_addr.sin_port));
	
	char packet[4096];
	memset(packet, 0, 4096);

	struct iphdr *iph = (struct iphdr *) packet;
	struct tcphdr *tcph = (struct tcphdr *) (packet + sizeof(struct iphdr));
	
	int data_len = 0;

    //Fill in the IP Header
	iph->ihl = 5;			
	iph->version = 4;															// IPv4
	iph->tos = 0;																// no special service
	iph->tot_len = sizeof (struct iphdr) + sizeof (struct tcphdr) + data_len;	// size of the whole packet
	iph->id = htons(getpid() % 65536);											// id of this packet
	iph->frag_off = 0;															// turn off fragmentation
	iph->ttl = 38;																// time to live
	iph->protocol = IPPROTO_TCP;												// TCP protocol
	iph->check = 0;																// set to 0 before calculating checksum
	iph->saddr = inet_addr(get_local_saddr());									// source IP adress
	iph->daddr = dst_addr.sin_addr.s_addr;										// destination IP adresss TODO:(inet_pton)
    iph->check = 0;										// if you set a checksum to zero, your 
	// kernel's IP stack should fill in the correct checksum during transmission					

	tcph->source = htons(rand() % 65536);	// source port is set to random number
	tcph->dest = htons(func_thread_arg->port);		// destination port 
	tcph->seq = htonl(getpid());					// random number
	tcph->ack_seq = 0;							// SYN packet
	tcph->doff = 5; 							// sizeof(struct tcphdr) / 4;	// Size of tcp header
	tcph->fin=0;
	tcph->syn=1;							// SYN
	tcph->rst=0;
	tcph->psh=0;
	tcph->ack=0;
	tcph->urg=0;
	tcph->window = htons(1024);				// maximum allowed window size
	tcph->check = 0; 						// kernel will fix this									
	tcph->urg_ptr = 0;						// urgency set to false
	
	// fprintf(stderr, "sending:  port_src: %6d, port_dst: %6d, %6d\n", ntohs(tcph->source), ntohs(tcph->dest), ntohs(func_thread_arg->port));
	if (sendto(sockfd, packet, iph->tot_len, 0, (struct sockaddr *)&dst_addr, sizeof(dst_addr)) == -1) {
		perror("[TCP, send to]");
		exit(1);
	}

	close(sockfd);
	// pthread_exit(NULL);
	return 0;
}


void tcp_scan(struct arguments_t *arguments) {
	printf("Local ip adress:  %s\n", get_local_saddr());
	printf("target ip adress: %s\n", arguments->ip_address);
	
	// TODO: when converted co cpp, change to vector
	int ports[MAXPORT];
	calculate_port_number(arguments->tcp_port_range, ports);
	
	struct thread_arg_t thread_arg;
	thread_arg.ip_adress = arguments->ip_address;
	thread_arg.interface = arguments->interface;
	// start pcap sniffer
	// scan ports
	printf("PORT STATE:\n");
	for (int i = 0; i < range; i++) {
		thread_arg.port = ports[i];
		sent_TCP(&thread_arg);
	}
}

