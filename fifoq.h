#include <ucontext.h>

#ifndef __FIFO_H__
#define __FIFO_H__


struct node {
	ucontext_t thread;
	int isMain; // main = 0, not main = 1.
	struct node* next;
};

void node_init(struct node* list);

int fifo_append(ucontext_t cthread, struct node **head);

void fifo_clear(struct node *list);

struct node* fifo_pop(struct node **list);

void fifo_push(struct node *list, struct node *thread);

#endif // __FIFO_H__