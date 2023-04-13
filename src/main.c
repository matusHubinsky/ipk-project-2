
#include <stdlib.h>
#include <argp.h>

#include "../inc/main.h"
#include "../inc/scanner.h"

struct arguments_t arguments;

const char *argp_program_version = "argp-ex3 1.0";
const char *argp_program_bug_address = "<bug-gnu-utils@gnu.org>";

/* Program documentation. */
static char doc[] = "\
This is a simple TCP and UDP network L4 scanner. The program will scan specified \
domain name or IP adresss (IPv4 or IPv6) and ports. It will output to stdout port\
statuses (open, filtered, closed).";

/* A description of the arguments we accept. */
static char args_doc[] = "idk....";

/* The options we understand. */
static struct argp_option options[] = {
	{"network interface",  	'i', 	  "interface",     		    0, "Just one interface to scan through" },
	{"pt",   		 		't', "tcp_port_range", 			    0, "Port-ranges - scanned tcp ports"	},
	{"pu",   		 		'u', "udp_port_range",  		    0, "Port-ranges - scanned udp ports"	},
	{"wait",   				'w',   	  "wait_time",   		    0, "Is the timeout in milliseconds"		},
	{"domain-name",	   		  0, 		    	0,	 OPTION_ALIAS, "Domain name"						},
	{"ip-address", 	   	      0, 		    	0,	 OPTION_ALIAS, "IP address"							},
	{"debug",	     		'd', 			 	0,	OPTION_HIDDEN, "Set program to debug mode"			},

  	{ 0 }
};


static void arg_print(struct arguments_t *arguments) {
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

/* Parse a single option. */
static error_t parse_opt (int key, char *arg, struct argp_state *state) {
	/* Get the input argument from argp_parse, which we know is a pointer to our arguments structure. */
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


int main (int argc, char *argv[]) {
	arguments.interface = NULL;
	arguments.tcp = false;
	arguments.udp = false;
	arguments.debug = false;
	arguments.tcp_port_range = NULL;
	arguments.udp_port_range = NULL;
	arguments.wait_time = -1;
	arguments.ip_address = NULL;

	struct argp argp = { options, parse_opt, args_doc, doc};
	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	
	if (arguments.debug) {
		arg_print(&arguments);
	}

	if (arguments.tcp) {
		tcp_scan(&arguments);
	}
		
	exit (0);
}
