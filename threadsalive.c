/*
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <strings.h>
#include <ucontext.h>
#include <string.h>
#include "threadsalive.h"
#include "fifoq.c"

#define STACKSIZE 8192

static ucontext_t* ready; // Because the ucontext structure has the link that basically
                          // acts like a queue on its own, Stratton suggested using it
                          // as the linked list instead of implementing our own.


/* ***************************** 
     stage 1 library functions
   ***************************** */

void ta_libinit(void) {
    printf("In init 1\n");
    ucontext_t* readyContext = malloc(sizeof(ucontext_t));
    ready = readyContext;
    printf("After malloc plz plz\n");
    getcontext(ready);
    ready -> uc_link = NULL;
    return;
}

void ta_create(void (*func)(void *), void *arg) {
  // Create node, make context, set context, add to queue.

    ucontext_t thread;
    unsigned char *stack = (unsigned char *)malloc(STACKSIZE);

    /* Set up thread*/
    getcontext(&thread);
    thread.uc_stack.ss_sp = stack;
    thread.uc_stack.ss_size = STACKSIZE;
    thread.uc_link = NULL;

    // Makes the context - switch into it using switch_context()
    makecontext(&thread, (void (*)(void))func, arg);
    fifo_push(ready, thread);
    return;
}

void ta_yield(void) { 
  ucontext_t *current = NULL;
  current = fifo_pop(&ready);
  fifo_push(ready, *current);
  swapcontext(current, current -> uc_link);

  if (current != ready){
    // The only way for us to get here is if the thread returned as opposed to
    // calling an explicit yield.
    ucontext_t *temp = NULL;
    temp = fifo_pop(&ready);
    free(&(temp -> uc_stack));
  }
  return;
}

int ta_waitall(void) {
  // Idea: Run all threads via yield until the queue is empty.
  // Only called from calling program - wait for all threads to finish.
    while (ready != NULL) {
      ta_yield();
      ready = ready -> uc_link;
    }
    return 0;
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

