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
static int tid = 0;
bool inMain = true;
static ucontext_t main;
main = get_context(main); 

/* ***************************** 
     stage 1 library functions
   ***************************** */


static void main_scheduler() {
  // While ready queue has anything in it, runs through ready queue every time
  // a yield is called. 
  while (1) {
    if (ready = NULL) {
      break;
    }
  }
} 


void ta_libinit(void) {
    
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

    fifo_append(thread, tid, ready);
    tid++;

    return;
}

void ta_yield(void) {
  // Check if you're in main, if you are switch to next ready thread.
  // If not, go to main and then recall.
  // Consider case where yield is called from a thread, consider where
  // yield is called from the calling program. 

    return;
}

int ta_waitall(void) {
  // Only called from calling program - wait for all threads to finish.
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

