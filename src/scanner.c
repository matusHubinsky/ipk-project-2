
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>


#include "../inc/scanner.h"

int client_socket;
int range = 0;

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
    // only one number
}


void tcp_scan(struct arguments_t *arguments) {
    // TODO: when converted co cpp, change to vector
    int ports[1024];

    calculate_port_number(arguments->port_range, ports);

    struct sockaddr_in target_adress;
    struct in_addr adress;
    inet_aton(arguments->ip_adress, &adress);

    printf("PORT STATE:\n");

    for (int i = 0; i < range; i++) {
        memset(&target_adress,0, sizeof(target_adress));
        target_adress.sin_family = AF_INET;
        target_adress.sin_port = htons(ports[i]);
        target_adress.sin_addr = adress;
    
        client_socket = socket(AF_INET, SOCK_STREAM, 0);

        int result = connect(client_socket, (const struct sockaddr *) &target_adress, sizeof(target_adress));

        usleep(arguments->wait_time * 1000);
 
        printf("%i/tcp ", ports[i]);
        if (result == -1 ) {
            printf("closed\n");
        } else if (result == 0) {
            printf("filtered\n");
        } else {
            printf("open\n");
        }
    }
}