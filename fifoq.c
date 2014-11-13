#include <stdio.h>
#include <stdlib.h>
#include "fifoq.h"

int fifo_append(ucontext_t cthread, struct node **head) {
    // Appends a new node containing the thread's context 
    // and that thread's identifying number.
    struct node *temp = malloc(sizeof(struct node));
    temp -> thread = cthread;
    temp -> isMain = 1;
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

void fifo_push(struct node *list, struct node *thread) {
    if (thread != NULL) {
        struct node *temp = list;
        while (temp -> next != NULL) {
            temp = temp -> next;
        }
        temp -> next = thread;
    }
    else {
        list = thread;
    }
}