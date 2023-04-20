/**
 * @file list.c
 * @author Matus Hubinsky
 * @brief data structure list and functions for it
 * @date 17.04.2023
 * @version 1.0.1
 */


#include "../inc/list.h"


struct list_t list;
enum state_e state;


/**
 * @brief return string variant of state from function paramater
 * 
 * @param state that we want to convert
 * @return string variant of state
*/
static char* print_state(state_e state) {
    if (state == OPEN) {
        return "open";
    } else if (state == CLOSED) {
        return "closed";
    } else {
        return "filtered";
    }
}


/**
 * @brief initialize list_t data structure
 * 
 * @param list list that will be initialized
 * @param range range of ports that will be added to list
 * @param ports array of ports that will be added to list
 * @param protocol list protocol
 * @return string variant of state
*/
void list_init(struct list_t *list, int range, int ports[], char* protocol) {
	list->top = list;
	list->next = NULL;
	// default state for all the ports is OPEN
	for (int i = 0; i < range; i++) {
		list_add(ports[i], protocol, OPEN, list);
	}
}


/**
 * @brief add port to list_t data structure
 * 
 * @param port that we want to add
 * @param protocol port procotol
 * @param state port state
 * @param list list where we want to add port
 * @return string variant of state
*/
void list_add(int port, char *protocol, state_e state, struct list_t *list) {
	// initialize new port
	struct list_t *new_port_status = malloc(sizeof(struct list_t));
	if (new_port_status == NULL) {
		fprintf(stderr, "Malloc failed\n");
		exit(1);
	}

	new_port_status->port = port;
	new_port_status->protocol = protocol;
	new_port_status->state = state;
	new_port_status->next = NULL;
	new_port_status->top = list->top;

	if (list->top == NULL) {
		// new port status is will be on the top of the list
		list->top = new_port_status;
	} else {
		// new port status will be on the bottom of the list
		while (list->next != NULL) {
			list = list->next;
		}
		list->next = new_port_status;	
	}
}


/**
 * @brief destroy list_t data structure
 * 
 * @param list list where we want to add port
 * @return string variant of state
*/
void list_destroy(struct list_t *list) {
	if (list == NULL) {
		return;
	}	
	list_destroy(list->next);
	free(list);
	list = NULL;
}


/**
 * @brief initialize list_t data structure
 * 
 * @param index index of port that we want change
 * @param state port state
 * @param list list where we want to change port state
 * @return
*/
void list_change_state_index(int index, state_e state, struct list_t *list) {
	int i = 0;
	// look for index in list
	while (list != NULL) {
		if (i == index) {
			// change state of the port
			list->state = state;
			return;
		}
		list = list->next;
		i++;
	}
}


/**
 * @brief print list_t data structure
 * 
 * @param list list to print
 * @return
*/
void list_print(struct list_t *list) {
	if (list == NULL) {
		fprintf(stderr, "Can't print empty list!\n");
		exit(1);
	}
	list = list->next;
    while (list != NULL) {
        printf("%5d", list->port);
        printf("/%s", list->protocol);
        printf(" %s", print_state(list->state));
        printf("\n");
    	list = list->next;
	}
}