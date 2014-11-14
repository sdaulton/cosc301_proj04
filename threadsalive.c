/*
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <ucontext.h>
#include <strings.h>
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
  
  /*
  ready = malloc(sizeof(struct node));
  ready -> thread = main;
  ready -> isMain = 0; // Need to check if main so you know when you're actually done. Queue should just have main when you're done.
  ready -> next = NULL; 
  */
  return;
}

void ta_create(void (*func)(void *), void *arg) {
  // Create node, make context, set context.
  // Add node to ready q, link it back to main.

    ucontext_t* thread = malloc(sizeof(ucontext_t));
    unsigned char *stack = (unsigned char *)malloc(STACKSIZE);
    assert(stack);

    int *i = (int *)arg;
    printf("create arg: %d\n", *i);

    /* Set up thread*/
    getcontext(thread);
    thread -> uc_stack.ss_sp = stack;
    thread -> uc_stack.ss_size = STACKSIZE;
    thread -> uc_link = &main;
    makecontext(thread, (void (*)(void))func, 1, arg);
    fifo_append(thread, &ready, tid);
    tid++;
    return;
}

void ta_yield(void) {
  // Switches to the next thread on the ready queue, pushing the current
  // thread to the back.
  struct node* next = fifo_pop(&ready);
  printf("yield next tid: %d, ready tid: %d\n", next->tid, ready->tid);
  printf("next: %p, ready: %p\n", next, ready);
  fifo_push(&ready, next);
  swapcontext(*(&next -> thread), *(&ready -> thread));
  return;
}

int ta_waitall(void) {
  // Only called from calling program - wait for all threads to finish.
  while (ready -> next != NULL) {
    struct node *next = fifo_pop(&ready);
    if (next -> tid == 0) {
      printf("Something works\n");
      struct node* tmp = next;
      free((&tmp -> thread -> uc_stack));
      free(tmp -> thread);
      free(tmp);
      next = next -> next;
    }
    //printf("1 next: %p, ready: %p\n", next, ready);
    //printf("next tid: %d, ready tid: %d\n", next->tid, ready->tid);
    fifo_push(&ready, next);
    swapcontext(&main, *(&next -> thread));
    //printf("2 next: %p, ready: %p\n", next, ready);
    next -> tid = 0; // 0 = Finished so free next time you encounter it.
    //printf("tid now is: %d\n", next -> tid);
    //printf("Swapped context\n");
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

