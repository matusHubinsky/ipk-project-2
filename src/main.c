
#include <stdlib.h>
#include <argp.h>
#include <stdbool.h>

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
	{"interface",  	'i', 		"eth0",     	   0, "Just one interface to scan through" },
	{"pt",   		't',  "port_range", 		   0, "Port-ranges - scanned tcp ports"},
	{"pu",   		'u',  "port_range", 		   0, "Port-ranges - scanned udp ports"},
	{"wait",   		'w',   "wait_time", 		   0, "Is the timeout in milliseconds"},
	{"domain-name",	 0, 		   	 0,	OPTION_ALIAS, "Domain name"},
	{"ip-address", 	 0, 		     0, OPTION_ALIAS, "IP address"},
	
  	{ 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments {
	char *args;
	char *eth0;       
	bool tcp, udp;         
	int port_range, wait_time;
	char *ip_adress;
};

static void arg_print(struct arguments *arguments) {
	fprintf(stdout, "[i]: %s\n", arguments->eth0);
	
	if (arguments->tcp) {
		fprintf(stdout, "[t]: %d\n", arguments->port_range);
	}
		
	if (arguments->udp) {
		fprintf(stdout, "[u]: %d\n", arguments->port_range);
	}

	fprintf(stdout, "[w]: %d\n", arguments->wait_time);
	fprintf(stdout, "[a]: %s\n", arguments->ip_adress);
}

/* Parse a single option. */
static error_t parse_opt (int key, char *arg, struct argp_state *state) {
	/* Get the input argument from argp_parse, which we know is a pointer to our arguments structure. */
	struct arguments *arguments = state->input;

	switch (key) {
		case 'i':
			arguments->eth0 = arg;
			break;

		case 't':
			arguments->tcp = true;
			arguments->port_range = atoi(arg);
			break;

		case 'u':
			arguments->udp = true;
			arguments->port_range = atoi(arg);
			break;

		case 'w':
			arguments->wait_time = atoi(arg);
			break;

    	case ARGP_KEY_NO_ARGS:
			argp_usage(state);
			break;

    	case ARGP_KEY_ARG:
			arguments->ip_adress = arg;
      		state->next = state->argc;
			break;

    	default:
      		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc};

int main (int argc, char *argv[]) {
	struct arguments arguments;

	/* Default values. */
	arguments.eth0 = NULL;
	arguments.tcp = false;
	arguments.udp = false;
	arguments.port_range = -1;
	arguments.wait_time = -1;
	arguments.ip_adress = NULL;

	/* Parse our arguments; every option seen by parse_opt will
		be reflected in arguments. */
	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	arg_print(&arguments);

	exit (0);
}