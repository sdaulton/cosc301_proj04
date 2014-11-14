/*
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <ucontext.h>
#include <strings.h>
#include <ucontext.h>
#include <string.h>
#include "threadsalive.h"
#include "fifoq.c"

#define STACKSIZE 128000

static struct node* ready = NULL;
static ucontext_t main;
static int tid = 1;

/* ***************************** 
     stage 1 library functions
   ***************************** */

void ta_libinit(void) {
  return;
}

void ta_create(void (*func)(void *), void *arg) {
  // Create node, make context, set context.
  // Add node to ready q, link it back to main.
    struct node *temp = malloc(sizeof(struct node));
    temp->tid = tid;
    tid++;
    unsigned char *stack = (unsigned char *)malloc(STACKSIZE);
    assert(stack);

    /* Set up thread*/
    getcontext(&temp->thread);
    temp->thread.uc_stack.ss_sp = stack;
    temp->thread.uc_stack.ss_size = STACKSIZE;
    temp->thread.uc_link = &main;
    makecontext(&temp->thread, (void (*)(void))func, 1, arg);
    fifo_push(&ready, temp);
    return;
}

void ta_yield(void) {
  // Switches to the next thread on the ready queue, pushing the current
  // thread to the back.
  assert(ready); // If ready is null then something is wrong, exit.
  struct node* current = fifo_pop(&ready);
  fifo_push(&ready, current);
  swapcontext(&current -> thread, &ready -> thread);
  return;
}

int ta_waitall(void) {
  // Idea: Run all threads via yield until the queue is empty.
  // Only called from calling program - wait for all threads to finish.
  while (ready != NULL) {
    swapcontext(&main, &ready -> thread);
    struct node* del = fifo_pop(&ready);
    node_destroy(del);
  }
  printf("End of wait all\n");
  if (ready == NULL) { // FIXME
    return 0;
  }
  return -1;
}


/* ***************************** 
     stage 2 library functions
   ***************************** */

void ta_sem_init(tasem_t *sem, int value) {
}

void ta_sem_destroy(tasem_t *sem) {
}

void ta_sem_post(tasem_t *sem) {
}

void ta_sem_wait(tasem_t *sem) {
}

void ta_lock_init(talock_t *mutex) {
}

void ta_lock_destroy(talock_t *mutex) {
}

void ta_lock(talock_t *mutex) {
}

void ta_unlock(talock_t *mutex) {
}


/* ***************************** 
     stage 3 library functions
   ***************************** */

void ta_cond_init(tacond_t *cond) {
}

void ta_cond_destroy(tacond_t *cond) {
}

void ta_wait(talock_t *mutex, tacond_t *cond) {
}

void ta_signal(tacond_t *cond) {
}

