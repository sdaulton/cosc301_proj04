#include <ucontext.h>

#ifndef __FIFO_H__
#define __FIFO_H__


struct node {
	ucontext_t thread;
	int tid;
};

void node_init(struct node* list);

int fifo_append(ucontext_t cthread, int tid, struct node **head);

void fifo_print(struct node **head);

void fifo_clear(struct node *list);

struct node* fifo_pop(struct node **list);

#endif // __FIFO_H__