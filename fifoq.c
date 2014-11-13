#include <stdio.h>
#include <stdlib.h>
#include "fifoq.h"

int fifo_append(ucontext_t cthread, struct node **head, int tid) {
    // Appends a new node containing the thread's context 
    // and that thread's identifying number.
    struct node *temp = malloc(sizeof(struct node));
    temp -> thread = cthread;
    temp -> isMain = 1;
    temp -> tid = tid; 
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

void fifo_clear(struct node *ready) {
	// Code by Professor Sommers from HW3, modified by Sam
    while (ready != NULL) {
        struct node *tmp = ready;
        ready = ready->next;
        free(tmp);
    }
}

// Code by Bria - pops the head of the list off and returns 
// it so that the thread it contains can be run. 
struct node* fifo_pop(struct node **ready) {
	if (*ready != NULL) {
		struct node *temp = *ready;
		*ready = (*ready) -> next;
		return temp;
	}
	else {
		return NULL;
	}
}

void fifo_push(struct node **ready, struct node *thread) {
    thread -> next = NULL;
    if (*ready != NULL) {
        struct node *temp = *ready;
        while (temp -> next != NULL) {
            temp = temp -> next;
        }
        temp -> next = thread;
    }
    else {
        *ready = thread;
    }
}