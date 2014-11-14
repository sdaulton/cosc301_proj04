#include <ucontext.h>

#ifndef __FIFO_H__
#define __FIFO_H__


struct node {
	ucontext_t* thread;
	int isMain; // main = 0, not main = 1.
	int tid;
	struct node* next;
};

void node_init(struct node* ready);

int fifo_append(ucontext_t* cthread, struct node **head, int tid);

void fifo_clear(struct node *ready);

struct node* fifo_pop(struct node **ready);

void fifo_push(struct node **ready, struct node *thread);

#endif // __FIFO_H__