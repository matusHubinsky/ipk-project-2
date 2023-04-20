/**
 * @file udp.c
 * @author Matus Hubinsky
 * @brief udp scanner, pcap capturing packets
 * @date 17.04.2023
 * @version 1.0.1
 */


#include <pcap/pcap.h>
#include <stdint.h>

#include "../inc/udp.h"

#define MAXPORT 65535  // maximum port to scan

// enum for better storing the port state
typedef enum state_e {
    OPEN = 0,
    CLOSED = 1,
    FILTERED = 2
} state_e;

int sock_udp = -1;
int udp_packet = 1; // udp packets order number
pthread_t udp_thread = 0; // pcap thread
pcap_t *pcap_udp;
struct list_t *list_udp; // list of all udp porst to be scanned


/**
 * @brief cancel thread, close pcap, close socket and gracefully exit
 * 
 * @param arguments void pointer to arguments_t data structure
 * @param arr innteger array where will be stored all the port numbers
 * @param type protocol type
 * @return
*/
int* calculate_port_number(struct arguments_t *arguments, int arr[], char* type) {    
	int *range;
	char* port_range;

	if (strcmp(type, "udp") == 0) {
		port_range = arguments->udp_port_range;
		range = &arguments->udp_port_range_count;
	} else if (strcmp(type, "tcp") == 0) {
		port_range = arguments->tcp_port_range;
		range = &arguments->tcp_port_range_count;
	} else {
		fprintf(stderr, "Unknown protocol type\n");
		exit(1);
	}

	// port number are separeted by ',' or '-'
	if (strchr(port_range, ',') != NULL) {
		// in case of ',' there are unknown amount of ports and we need to store them all
		(*range) = 0;
		char* token;
		while ((token = strsep(&port_range, ",")) != NULL) {
			arr[*range] = atoi(token);
			(*range)++;
		}
	} else if (strchr(port_range, '-') != NULL) {
		// in case of '-' there are only two numbers, which are the range of ports
		char *search = "-";
		int from = atoi(strtok(port_range, search));
		int to = atoi(strtok(NULL, search));
		// fill list
		for (int i = 0; i < (to - from); i++) {
			arr[i] = i + from;
			(*range)++;
		}
	} else {
		// last case is one port only
		arr[0] = atoi(port_range);
		(*range) = 1;
	}

	return arr;
}


/**
 * @brief convert number of protocol to a string
 * 
 * @param protocol type of protocol
 * @return string value of protocol
*/
char* get_protocol(int protocol) {
	switch (protocol) {
		case IPPROTO_TCP:
			return "tpc";
		case IPPROTO_UDP:
			return "udp";
		case IPPROTO_ICMP:
			return "icmp";
		case IPPROTO_ICMPV6:
			return "icmpv6";
		default:
			return "Unknown";
	}
}			


/**
 * @brief cancel thread, close pcap, close socket and gracefully exit
 * 
 * @param arguments void pointer to arguments_t data structure
 * @return
*/
static void signal_handler(int signal) {
    if (signal == 2) {
		pthread_cancel(udp_thread);
		pcap_close(pcap_udp);
		close(sock_udp);
		exit(1);
    }
}


/**
 * @brief concatenate two strings
 * 
 * @param string1 first string to concatenate
 * @param string2 second string to concatenate
 * @return final string
*/
char *concatenate(const char *string1, const char *string2) {
	char *result = malloc(strlen(string1) + strlen(string2) + 1);
	strcpy(result, string1);
	strcat(result, string2);
	return result;
}


/**
 * @brief initialize pcap, set different filters for IPv4 and IPv6
 * 
 * @param arguments void pointer to arguments_t data structure
 * @return void pointer
*/
pcap_t* init_pcap(struct arguments_t *arguments) {
    char error_buffer[PCAP_ERRBUF_SIZE];
    struct bpf_program filter;
	char *filter_string = NULL;
	
	if (arguments->IPv4) {
		// filter only ICMP packets from targets ip address
		filter_string = concatenate("icmp and src ", arguments->ip_address);
	} else {
		// set no filter for IPv6
		filter_string = concatenate("", "");
	}

	bpf_u_int32 subnet_mask, ip;

    if (pcap_lookupnet(arguments->interface, &ip, &subnet_mask, error_buffer) == -1) {
		perror("pcap_lookupnet");
        exit(1);
    }
	
    pcap_udp = pcap_open_live(arguments->interface, BUFSIZ, 1, 1, error_buffer);
    if (pcap_udp == NULL) {
        perror("pcap_open_live");
        exit(1);
    }

    if (pcap_compile(pcap_udp, &filter, filter_string, 0, ip) == -1) {
        perror("pcap_compile");
        exit(1);
    }

    if (pcap_setfilter(pcap_udp, &filter) == -1) {
        perror("pcap_setfilter");
        exit(1);
    }

	free(filter_string);
	return pcap_udp;
}


/**
 * @brief process catched packets, in case of ICMP or ICMPv6 mark port as CLOSED
 * 
 * @param arguments void pointer to arguments_t data structure
 * @return void pointer
*/
void* packet_pcap_udpr(unsigned char *args, const struct pcap_pkthdr *header, const unsigned char *packet) {
	struct icmphdr *icmp = (struct icmphdr *)(packet + sizeof(struct ethhdr) + sizeof(struct iphdr));
	struct icmp6_hdr *icmp6 = (struct icmp6_hdr *)(packet + sizeof(struct ethhdr) + sizeof(struct ip6_hdr));

	// ICMP	
	if ((icmp->type == ICMP_DEST_UNREACH) && (icmp->code == ICMP_PORT_UNREACH)) {
		// mark port as CLOSED
		list_change_state_index(udp_packet, CLOSED, list_udp);
	}

	// ICMPv6
	if ((icmp6->icmp6_type == ICMP6_DST_UNREACH) && (icmp6->icmp6_code == 4)) {
		// mark port as CLOSED
		list_change_state_index(udp_packet, CLOSED, list_udp);
	}

	pthread_join(udp_thread, NULL);
	return NULL;
}


/**
 * @brief thread for pcap
 * 
 * @param arguments void pointer to arguments_t data structure
 * @return void pointer
*/
void* pcap_thread(void* arg) {
	struct arguments_t* arguments = (struct pcap_args_t*)arg;
	pcap_t *pcap_udp = init_pcap(arguments);
	pcap_loop(pcap_udp, -1, packet_pcap_udpr, NULL);
    pcap_close(pcap_udp);
    pthread_exit(NULL);
}


/**
 * @brief scan UDP ports
 * 
 * @param arguments arguments_t data structure
 * @return
*/
void udp_scan(struct arguments_t *arguments) {
	// initialize ports array
	int ports[MAXPORT];
	calculate_port_number(arguments, ports, "udp");

	unsigned char udp_buffer[2];

	struct sockaddr_in dst_addr_ipv4;
	memset(&dst_addr_ipv4, 0, sizeof(dst_addr_ipv4));

	struct sockaddr_in6 dst_addr_ipv6;
	memset(&dst_addr_ipv6, 0, sizeof(dst_addr_ipv6));

	// initialize socket and destination address for IPv4
	if (arguments->IPv4 == true) {
		sock_udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		dst_addr_ipv4.sin_family = AF_INET;
		dst_addr_ipv4.sin_addr.s_addr = inet_addr(arguments->ip_address);
	} 
	
	// initialize socket and destination address for IPv6
	if (arguments->IPv6 == true) {
		sock_udp = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
		dst_addr_ipv6.sin6_family = AF_INET6;
		inet_pton(AF_INET6, arguments->ip_address, &dst_addr_ipv6.sin6_addr);
	}

	// check socket state
	if (sock_udp < 0) {
		perror("socket");
		close(sock_udp);
		exit(1);
	}

	// set network interface for socket
	if (arguments->interface != NULL) {
		setsockopt(sock_udp, SOL_SOCKET, SO_BINDTODEVICE, arguments->interface, strlen(arguments->interface));
	}

	// create and initialize list_t data structure names list_udp
	list_udp = malloc(sizeof(struct list_t*));
	list_init(list_udp, arguments->udp_port_range_count, ports, "udp");

	pthread_create(&udp_thread, NULL, pcap_thread, (void*)arguments);
	
	// main port scanning
	int resoult = 0;
	for (int i = 0; i < arguments->udp_port_range_count; i++) {
		dst_addr_ipv4.sin_port = htons(ports[i]);
		dst_addr_ipv6.sin6_port = htons(ports[i]);

		// send UDP IPv4 message
		if (arguments->IPv4) {
			resoult = sendto(sock_udp, udp_buffer, sizeof(udp_buffer), 0, (struct sockaddr *)&dst_addr_ipv4, sizeof(dst_addr_ipv4));
		}

		// send UDP IPv6 message
		if (arguments->IPv6) {
			resoult = sendto(sock_udp, udp_buffer, sizeof(udp_buffer), 0, (struct sockaddr *)&dst_addr_ipv6, sizeof(dst_addr_ipv6));
		}

		if (resoult < 0) {
			perror("sendto");
			exit(1);
		} 

		// wait for a specified amount of time (from user or default value) 
		usleep(1000 * arguments->wait_time);
		udp_packet++;
	}
	
    close(sock_udp);

	list_print(list_udp);
	list_destroy(list_udp);
}

