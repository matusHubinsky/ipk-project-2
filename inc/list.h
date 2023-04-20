/**
 * @file list.h
 * @author Matus Hubinsky
 * @brief 
 * @date 17.04.2023
 * @version 1.0.1
 */


#ifndef SCANNER_H_
#define SCANNER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum state_e {
    OPEN = 0,
    CLOSED = 1,
    FILTERED = 2
} state_e;

extern struct list_t list;
typedef struct list_t {
	int port;
	char *protocol;
	state_e state;
	struct list_t *top;
	struct list_t *next;
} list_t;


extern void list_init(struct list_t *port_list, int range, int ports[], char* protocol);

extern void list_add(int port, char *protocol, state_e state, struct list_t *port_status_ptr);

extern void list_destroy(struct list_t *port_status_ptr);

extern void list_change_state_index(int index, state_e state, struct list_t *list);

extern void list_print(struct list_t *port_status_ptr);

#endif