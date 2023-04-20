/**
 * @file main.c
 * @author Matus Hubinsky
 * @brief main
 * @date 17.04.2023
 * @version 1.0.1
 */


#include "../inc/main.h"


/**
 * @brief check if network interface exists, in case of not existing, sets first network interface
 * 
 * @param arguments data structure arguments_t with input arguments
 * @return
*/
static void check_network_interface(struct arguments_t* arguments) {
	bool exist = false;
	struct ifaddrs *addrs;

	getifaddrs(&addrs);

	while (addrs) {
		if (addrs->ifa_addr->sa_family == AF_PACKET)
			if (arguments->interface == NULL) {
				printf("%s\n", addrs->ifa_name);
			} else if (strcmp(arguments->interface, addrs->ifa_name) == 0) {
				exist = true;
			}

		addrs = addrs->ifa_next;
	}
	freeifaddrs(addrs);

	if (arguments->interface == NULL) {
		return;
	}

	if (!exist) {
		fprintf(stderr, "Unknown network interface\n");
		exit(1);
	}
}


/**
 * @brief converts domain name into IP address
 * 
 * @param arguments data structure arguments_t with input arguments
 * @return
*/
static void domain_to_adress(struct arguments_t* arguments) {
	struct hostent *host_info = (struct hostent*) gethostbyname(arguments->ip_address);
	// in case of IPv6
	if (host_info) {
		arguments->ip_address = inet_ntoa(*((struct in_addr *) (host_info->h_addr)));
	}
}


/**
 * @brief detect type od a IP address and returns its number (4 or 6)
 * 
 * @param arguments data structure arguments_t with input arguments
 * @return IPv4 return 4, IPv6 return 6
*/
static int detect_ip_adress_version(struct arguments_t* arguments) {
	char buffer[16];
	if (inet_pton(AF_INET, arguments->ip_address, buffer)) {
		arguments->IPv4 = true;
		return 4;
	} else if (inet_pton(AF_INET6, arguments->ip_address, buffer)) {
		arguments->IPv6 = true;
		return 6;
	} else {
		fprintf(stderr, "Wrong format of IP address\n");
		exit(1);
	}
}


/*
 * @brief call all the functions to run a program
 * 
 * @bugs TCP scanner not implemented yet
 * @param argc argument count
 * @param argv
 * @return 0 in case of success
*/
int main (int argc, char *argv[]) {
	// check OS type
	if (strcmp(OS, "linux") != 0) {
        fprintf(stderr, "ERROR: Program is NOT supported on this OS!\n");
		fprintf(stderr, "Program is running ONLY on LINUX.\n");
        return 1;
	}

	// parse arguments
	struct argp argp = {options, parse_opt, NULL, doc};
	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	
	if (arguments.debug) {
		arg_print(&arguments);
	}

	check_network_interface(&arguments);
	domain_to_adress(&arguments);
	detect_ip_adress_version(&arguments);
	
	printf("\n");
	printf("Interesting ports on (%s):\n", arguments.ip_address);
	printf("PORT      STATE\n");

	// TCP scanner not implemented yet
	if (arguments.tcp) {
		// tcp_scan(&arguments);
	}

	// call UDP scanner
	if (arguments.udp) {
		udp_scan(&arguments);
	}

	return 0;
}