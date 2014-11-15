#include <ucontext.h>
#include "threadsalive.h"
#ifndef __FIFO_H__
#define __FIFO_H__


struct node {
	ucontext_t thread;
	int tid;
	struct node* next;
};

struct locknode {
    talock_t *lock;
    struct locknode* next;
};

struct semnode {
    tasem_t *sem;
    struct semnode* next;
};

void node_init(struct node* ready);

void fifo_print(struct node *ready);

void fifo_clear(struct node *ready);

struct node* fifo_pop(struct node **ready);

void fifo_push(struct node **ready, struct node *thread);

void node_destroy(struct node *ready);

void locknode_push(struct locknode **locklist, struct locknode *lock);

struct locknode* locknode_pop(struct locknode **locklist);

void semnode_push(struct semnode **semlist, struct semnode *sem);

struct semnode* semnode_pop(struct semnode **semlist);



#endif // __FIFO_H__
