#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "fifoq.h"

void fifo_print(struct node *ready) {
    // Original code by Professor Sommers, modified by Bria.
    printf("***List contents begin***\n");
    while (ready != NULL) {
        printf("tid: %d\n", ready -> tid);
        ready = ready -> next;
    }
    printf("***List contents end***\n");
}

void fifo_clear(struct node *ready) {
	// Code by Professor Sommers from HW3, modified by Sam
    while (ready != NULL) {
        struct node *tmp = ready;
        ready = ready->next;
        free(tmp);
    }
}
*/

// Code by Bria - pops the head of the list off and returns
// it so that the thread it contains can be run. 
struct node* fifo_pop(struct node **ready) {
    struct node* temp = *ready;
	if (temp != NULL) {
        *ready = (*ready) -> next;
	}
    return temp;
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


void node_destroy(struct node *thread) {
    free(thread -> thread.uc_stack.ss_sp);
    free(thread);
}
