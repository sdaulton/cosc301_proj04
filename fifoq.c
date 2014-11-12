#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "fifoq.h"

// All done by Bria.

/*
int fifo_append(ucontext_t thread, ucontext_t* ready) {
    // Links the next thread into the ready queue.
    // Assumes that the new thread is linked to NULL already.
    if (ready == NULL) {
        ready = &thread;
    }
    else {
        ucontext_t* temp = ready;
        while (temp != NULL) {
            temp = temp -> uc_link;
        }
        temp.uc_link = &thread;
    }
}
*/

/*
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
*/

// Code by Bria - pops the head of the list off and returns 
// it so that it can be run. 
ucontext_t* fifo_pop(ucontext_t **list) {
	if (*list != NULL) {
		ucontext_t* temp = *list;
		*list = (*list) -> uc_link;
		return temp;
	}
	else {
		return NULL;
	}
}

// Code by Bria - pushes the already existing node to the back of
// the list (to the end of the queue).
void fifo_push(ucontext_t *list, ucontext_t *thread) {
    thread->uc_link = NULL;
    if (list == NULL) {
        list = thread;
        return;
    }
    while (list -> uc_link != NULL) {
        list = list -> uc_link;
    }
    list -> uc_link = thread;
}
