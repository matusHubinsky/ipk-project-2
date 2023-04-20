/**
 * @file arg.c
 * @author Matus Hubinsky
 * @brief argument parser
 * @date 17.04.2023
 * @version 1.0.1
 */


#include "../inc/arg.h"


/* data structure stroring arguments */
struct arguments_t arguments = {	
    .interface = NULL,
	.tcp = false,
	.udp = false,
	.debug = false,
	.tcp_port_range = NULL,
	.udp_port_range = NULL,
	.wait_time = 5000,
	.ip_address = NULL,
	.IPv4 = false,
	.IPv6 = false
};


/* simple program documentation */
char doc[] = "\
This is a simple TCP and UDP network L4 scanner. The program will scan specified \
domain name or IP adresss (IPv4 or IPv6) and ports. It will output to stdout port\
statuses (open, filtered, closed).";


/* data struccture with all the arguments, their variants and description */
struct argp_option options[] = {
	{"network interface",  	'i', 	  "interface",     		    0, "Just one interface to scan through" ,0},
	{"pt",   		 		't', "tcp_port_range", 			    0, "Port-ranges - scanned tcp ports"	,0},
	{"pu",   		 		'u', "udp_port_range",  		    0, "Port-ranges - scanned udp ports"	,0},
	{"wait",   				'w',   	  "wait_time",   		    0, "Is the timeout in milliseconds"		,0},
	{"domain-name",	   		  0, 		    	0,	 OPTION_ALIAS, "Domain name"						,0},
	{"ip-address", 	   	      0, 		    	0,	 OPTION_ALIAS, "IP address"							,0},
	{"debug",	     		'd', 			 	0,	OPTION_HIDDEN, "Set program to debug mode"			,0},

  	{ 0 }
};


/**
 * @brief detect type od a IP address and returns its number (4 or 6)
 * 
 * @param arguments data structure arguments_t with input arguments
 * @return IPv4 return 4, IPv6 return 6
*/
void arg_print(struct arguments_t *arguments) {
	fprintf(stdout, "[i]: %s\n", arguments->interface);
	
	if (arguments->tcp) {
		fprintf(stdout, "[t]: %s\n", arguments->tcp_port_range);
	}
		
	if (arguments->udp) {
		fprintf(stdout, "[u]: %s\n", arguments->udp_port_range);
	}

	fprintf(stdout, "[w]: %d\n", arguments->wait_time);
	fprintf(stdout, "[a]: %s\n", arguments->ip_address);
}


/**
 * @brief parse all the arguments and store them in the data structure
 * 
 * @param key argument options
 * @param arg argument value
 * @param state arguments data structure
 * @return error in case of wrong number of arguments
*/
error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments_t *arguments = state->input;

	switch (key) {
		case 'd':
			arguments->debug = true;
			break;

		case 'i':
			arguments->interface = arg;
			break;

		case 't':
			arguments->tcp = true;
			arguments->tcp_port_range = arg;
			break;

		case 'u':
			arguments->udp = true;
			arguments->udp_port_range = arg;
			break;

		case 'w':
			arguments->wait_time = atoi(arg);
			break;

    	case ARGP_KEY_NO_ARGS:
			argp_usage(state);
			break;

    	case ARGP_KEY_ARG:
			arguments->ip_address = arg;
      		state->next = state->argc;
			break;

    	default:
      		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}
