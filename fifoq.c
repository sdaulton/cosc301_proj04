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

// Code by Bria - pops the head of the list off and returns
// it.
struct node* fifo_pop(struct node **ready) {
    struct node* temp = *ready;
    if (temp != NULL) {
            *ready = (*ready) -> next;
    }
    return temp;
}

// Code by Bria - pushes an already existing node to the
// back of the list.
void fifo_push(struct node **ready, struct node *thread) {
    if (thread == NULL) {
        return;
    }
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

// Code by Bria - frees a thread node.
void node_destroy(struct node *thread) {
    free(thread -> thread.uc_stack.ss_sp);
    free(thread);
}


// Taken from Bria and adapted by Sam - pops the head of the list off and returns
// it so that the threads can be removed from locks' and semaphores' waiting lists it contains can be run. 
// the remove methods remove the node with the matching id from the list parameter and returns a pointer to that node
struct locknode* locknode_pop(struct locknode **locklist) {
    struct locknode* temp = *locklist;
    if (temp != NULL) {
            *locklist = (*locklist) -> next;
    }
    return temp;
}

void locknode_push(struct locknode **locklist, struct locknode *lock) {
    lock -> next = NULL;
    if (*locklist != NULL) {
        struct locknode *temp = *locklist;
        while (temp -> next != NULL) {
            temp = temp -> next;
        }
        temp -> next = lock;

    }
    else {
        *locklist = lock;

    }
}

struct locknode* locknode_remove(struct locknode **locklist, int lockid) {
    if (*locklist != NULL) {
        struct locknode *last = *locklist;
        struct locknode *next = last->next;
        if ((last->lock)->lockid == lockid) {
            *locklist = next;
            return last;
        }
        while (next != NULL) {
            
            if((next->lock)->lockid == lockid) {
                last->next = next->next;
                return next;
            }
            last = next;
            next = next->next;
        }
    }
    return NULL;
}

struct semnode* semnode_pop(struct semnode **semlist) {
    struct semnode* temp = *semlist;
    if (temp != NULL) {
            *semlist = (*semlist) -> next;
    }
    return temp;
}

void semnode_push(struct semnode **semlist, struct semnode *sem) {
    sem -> next = NULL;
    if (*semlist != NULL) {
        struct semnode *temp = *semlist;
        while (temp -> next != NULL) {
            temp = temp -> next;
        }
        temp -> next = sem;

    }
    else {
        *semlist = sem;

    }
}

struct semnode* semnode_remove(struct semnode **semlist, int semid) {
    if (*semlist != NULL) {
        struct semnode *last = *semlist;
        struct semnode *next = last->next;
        if ((last->sem)->semid == semid) {
            *semlist = next;
            return last;
        }
        while (next != NULL) {
            
            if((next->sem)->semid == semid) {
                last->next = next->next;
                return next;
            }
            last = next;
            next = next->next;
        }
    }
    return NULL;
}
