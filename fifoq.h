#include <ucontext.h>

#ifndef __FIFO_H__
#define __FIFO_H__

ucontext_t* fifo_pop(ucontext_t **list);
void fifo_push(ucontext_t *list, ucontext_t *thread);

struct node {
	ucontext_t thread;
	int tid;
	struct node* next;
};

void node_init(struct node* ready);

void fifo_print(struct node *ready);

void fifo_clear(struct node *ready);

struct node* fifo_pop(struct node **ready);

void fifo_push(struct node **ready, struct node *thread);

void node_destroy(struct node *ready);

#endif // __FIFO_H__
