/**
 * @file list.h
 * @author Matus Hubinsky
 * @brief 
 * @date 17.04.2023
 * @version 1.0.1
 */


#ifndef ARG_H_
#define ARG_H_

#include <stdlib.h>
#include <argp.h>
#include <stdbool.h>

#define OPT_ABORT  1

extern struct arguments_t arguments;
struct arguments_t {
	char *args;
	char *interface;       
	bool tcp, udp, debug;         
	char *tcp_port_range, *udp_port_range; 
	int wait_time;
    char *ip_address;
    int tcp_port_range_count, udp_port_range_count;
	bool IPv4;
	bool IPv6;
};

extern char doc[];
extern struct argp_option options[];

extern void arg_print(struct arguments_t *arguments);
extern error_t parse_opt (int key, char *arg, struct argp_state *state);

#endif