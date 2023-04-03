
#ifndef MAIN_H_
#define MAIN_H_

#include <stdbool.h>

/* Keys for options without short-options. */
#define OPT_ABORT  1            /* –abort */

// TODO: port range for udp and tcp 
struct arguments_t {
	char *args;
	char *eth0;       
	bool tcp, udp, debug;         
	char *port_range; 
	int wait_time;
    char *ip_adress;
};
extern struct arguments_t arguments;

#endif