#include <stdio.h>
#include <stdlib.h>
#include "fifoq.h"

// All modified from Sam's pid list functions from Proj 02.

int fifo_append(ucontext_t cthread, int ctid, struct node **current, int tcount) {
    // Appends a new node containing the thread's context 
    // and that thread's identifying number.
    struct node *temp = malloc(sizeof(struct node));
    temp -> thread = cthread;
    temp -> tid = ctid;
    temp -> next = NULL;
    if (*head != NULL) {
        struct node *list = *head;
        while (list -> next != NULL) {
            list = list -> next;
        }
        list -> next = temp;
    }
    else {
        *head = temp;
    }
    return 0;
}

void fifo_print(struct node **head) {
    // takes a pointer to a ponter to the head of a linked list of thread nodes
    // and prints the list of threads based on their thread id #
    struct node *next_node = *head;
    printf("***Current threads start***\n");
    while (next_node != NULL) {
        printf("Process: %d\n", next_node->tid);
        next_node = next_node -> next;
    }
    printf("***Current threads end***\n");
}

void fifo_clear(struct node *list) {
	// Code by Professor Sommers from HW3, modified by Sam
    while (list != NULL) {
        struct node *tmp = list;
        list = list->next;
        free(tmp);
    }
}

// Code by Bria - pops the head of the list off and returns 
// it so that the thread it contains can be run. 
struct node* fifo_pop(struct node **list) {
	if (*list != NULL) {
		struct node *temp = *list;
		*list = (*list) -> next;
		return temp;
	}
	else {
		return NULL;
	}
}