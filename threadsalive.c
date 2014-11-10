/*
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <strings.h>
#include <string.h>
#include "threadsalive.h"
#include "fifoq.c"

#define STACKSIZE 8192

static struct node* ready = NULL;
int tid = 0;
int currentTID = 0;
static int inMain = 0;
static ucontext_t main;
main = get_context(main); 

/* ***************************** 
     stage 1 library functions
   ***************************** */

void ta_libinit(void) {
    struct node* mainNode;
    mainNode.thread = main;
    mainNode.tid = 0;
    mainNode.active = 1;
    return;
}

void ta_create(void (*func)(void *), void *arg) {
  // Create node, make context, set context.
  // Add node to ready q, link it back to main.
  // Give it thread ID & function.

    ucontext_t thread = NULL;
    unsigned char *stack = (unsigned char *)malloc(STACKSIZE);

    /* Set up thread*/
    get_context(&thread);
    thread.uc_stack.ss_sp = stack;
    thread.uc_stack.ss_size = STACKSIZE;
    thread.uc_link = &main;

    // Makes the context - switch into it using switch_context()
    make_context(thread, func, arg);
    fifo_append(thread, tid, ready);

    tid++;
    return;
}

void ta_yield(void) { 
  // If you are not in main, this means you are running the current head
  // of the ready queue so swap to main.
  if (!inMain) {
    swap_context(ready.thread, &main);
    fifo_append()
  }

  // If you are in main, run the head of the ready queue.
  else {
    if (ready == NULL) {
      return;
    }

    inMain = 0;
    ready = ready -> next;
    swap_context(&main, &(ready.thread));
    inMain = 1;


    // WHEN TO FREE THREADS? Figure out how to determine when thread is
    // done running, and then free the stack and remove the node.
  }


  return;
}

int ta_waitall(void) {
  // Idea: Run all threads via yield until the queue is empty.
  // Only called from calling program - wait for all threads to finish.
    while (ready != NULL) {
      ta_yield(ready);
      ready = ready -> next;
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

