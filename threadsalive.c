/*
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <ucontext.h>
#include <strings.h>
#include <errno.h>
#include <string.h>
#include "threadsalive.h"
#include "fifoq.c"

#define STACKSIZE 128000

/* CHECKLIST: 
- Valgrind checks out, no mem leaks.
- Sys calls?
- Asserts?
*/

static struct node* ready = NULL;
static ucontext_t main;
static int tid = 1;
static struct locknode *locklist = NULL;
static struct semnode *semlist = NULL;

/* ***************************** 
     static helper functions
     Written by Sam.
   ***************************** */

// destroy threads that are waiting on a semaphore (including the semaphore guards on mutexs) and free the semnode
// and returns the number of threads that were blocked
static int ta_sem_thread_clearer(void) {
    int numkilled = 0;
    while (semlist != NULL) {
        struct semnode *del_sem = semnode_pop(&semlist);
        tasem_t *sem = del_sem->sem;
        while(sem->waiting_q != NULL) {
            struct node *del = fifo_pop(&(sem->waiting_q));
            node_destroy(del);
            numkilled++;
        }
        free(del_sem);
    }
    return numkilled;
}

// destroy threads that are waiting on a mutex and free the locknode and returns the number of threads that were blocked
static int ta_lock_thread_clearer(void) {
    int numkilled = 0;
    while (locklist != NULL) {
        struct locknode *del_lock = locknode_pop(&locklist);
        talock_t *mutex = del_lock->lock;
        while(mutex->waiting_q != NULL) {
            struct node *del = fifo_pop(&(mutex->waiting_q));
            node_destroy(del);
            numkilled++;
        }
        free(del_lock);
    }
    return numkilled;
}


//atomic test and set from class
static int testAndSet(int *ptr, int new) {
    int old = *ptr;
    *ptr = new;
    return old;
}

/* ***************************** 
     stage 1 library functions
     Code by Bria, modifications
     that implement later stage 
     functionality by Sam.
   ***************************** */

void ta_libinit(void) {
  return;
}

void ta_create(void (*func)(void *), void *arg) {
  // Create node, make context, set context.
  // Add node to ready q, link it back to main.
    struct node *temp = malloc(sizeof(struct node));
    assert(temp);
    temp->tid = tid;
    tid++;
    unsigned char *stack = (unsigned char *)malloc(STACKSIZE);
    assert(stack);

    /* Set up thread*/
    int check = getcontext(&temp->thread);
    if (check == -1) {
      fprintf(stderr, "Get context failed!\nReason: %s\n", strerror(errno));
    }
    temp->thread.uc_stack.ss_sp = stack;
    temp->thread.uc_stack.ss_size = STACKSIZE;
    temp->thread.uc_link = &main;
    makecontext(&temp->thread, (void (*)(void))func, 1, arg); // Failure of make context?
    fifo_push(&ready, temp);
    return;
}

void ta_yield(void) {
  // Switches to the next thread on the ready queue, pushing the current
  // thread to the back.
  assert(ready);
  struct node* current = fifo_pop(&ready);
  fifo_push(&ready, current);
  int check = swapcontext(&current -> thread, &ready -> thread);
  if (check == -1) {
    fprintf(stderr, "Swap context failed!\nReason: %s\n", strerror(errno));
  }
  return;
}

int ta_waitall(void) {
  // Only called from calling program - wait for all threads to finish.
  int numblockedkilled = 0;
  while (ready != NULL) {
    int check = swapcontext(&main, &ready -> thread);
    if (check == -1) {
      fprintf(stderr, "Swap context failed!\nReason: %s\n", strerror(errno));
    }
    // Process at head of list just finished, clear it.
    struct node* del = fifo_pop(&ready);
    node_destroy(del);
  }
  int numkilled = ta_sem_thread_clearer();
  numblockedkilled += numkilled;
  numkilled = ta_lock_thread_clearer();
  numblockedkilled += numkilled;
  return -1 * numblockedkilled;
}


/* ***************************** 
     stage 2 library functions
     Code by Sam.
   ***************************** */


//initializes the semaphore and creates an semnode which is used by waitall to destroy any threads that are blocked--waiting
//for the semaphore when waitall is called
void ta_sem_init(tasem_t *sem, int value) {
    sem->value = value;
    sem->waiting_q = NULL;
    sem->guard = 0;
    struct semnode *newsema = malloc(sizeof(struct semnode));
    assert(newsema);
    newsema->sem = sem;
    semnode_push(&semlist, newsema);
}

// does nothing 
void ta_sem_destroy(tasem_t *sem) {
}

// after getting past guard (mutex) for semaphore, increments the value and wakes up a thread if there are 
// any threads waiting for the semaphore
void ta_sem_post(tasem_t *sem) {
    while(testAndSet(&sem->guard, 1) == 1) {};
    sem->value++;
    if (sem->value <= 0) {
        //there are threads waiting
        struct node *woken_thread = fifo_pop(&sem->waiting_q);
        fifo_push(&ready, woken_thread);
    }
    sem->guard = 0;
}

//after getting past guard (mutex) for the semaphore, decrements the value.  If the semaphore is in use,
//the thread adds itself to the waiting queue and gives up the CPU.  Otherwise it "takes" the semaphore
void ta_sem_wait(tasem_t *sem) {
    while(testAndSet(&sem->guard, 1) == 1) {};
    sem->value--;
    if (sem->value < 0) {
        // remove current thread from ready queue
        // add current thread to wait list for semaphore
        struct node *current = fifo_pop(&ready);
        fifo_push(&sem->waiting_q, current);
        sem->guard = 0;
        // give up CPU
        int check = swapcontext(&current -> thread, &ready -> thread);
        if (check == -1) {
          fprintf(stderr, "Swap context failed!\nReason: %s\n", strerror(errno));
        }

    } else {
        sem->guard = 0; 
    }
}

//initializes the lock and creates an locknode which is used by waitall to destroy any threads that are blocked--waiting
//for the lock when waitall is called
void ta_lock_init(talock_t *mutex) {
    ta_sem_init(&mutex->sem, 1);
    mutex->flag = 0;
    mutex->waiting_q = NULL;
    struct locknode *newlock = malloc(sizeof(struct locknode));
    assert(newlock);
    newlock->lock = mutex;
    locknode_push(&locklist, newlock);
}

//does nothing
void ta_lock_destroy(talock_t *mutex) {
}

// uses a semaphore as a guard on the lock.  Once thread has the semaphore, if the lock is free: the thread takes the lock
// and releases the semaphore.  If the lock is taken, the thread adds itself to a waiting queue for the lock and releases
// the semaphore
void ta_lock(talock_t *mutex) {
    ta_sem_wait(&mutex->sem);
    if (mutex->flag == 0) {
        // lock is free
        mutex->flag = 1;
        ta_sem_post(&mutex->sem);
    } else {
        struct node *current = fifo_pop(&ready);
        fifo_push(&mutex->waiting_q, current);
        //numblockedthreads++;
        ta_sem_post(&mutex->sem);
        int check = swapcontext(&current -> thread, &ready -> thread);
        if (check == -1) {
          fprintf(stderr, "Swap context failed!\nReason: %s\n", strerror(errno));
        }
    }
}

//uses a semaphore as a guard on the lock.  Once the thread has the semaphore, if there are threads waiting for the lock:
// it wakes the next thread.  If there are no threads waiting, it releases the lock.  Then it releases the semaphore
void ta_unlock(talock_t *mutex) {
    ta_sem_wait(&mutex->sem);
    if (mutex->waiting_q == NULL) {
        // no threads waiting for lock
        mutex->flag = 0;
    } else {
        // give lock to next thread waiting for lock
        struct node *next = fifo_pop(&mutex->waiting_q);
        fifo_push(&ready, next);
    }
    ta_sem_post(&mutex->sem);
}



/* ***************************** 
     stage 3 library functions
     Code by Sam.
   ***************************** */

//initialize a semaphore with value = 0 so that all threads will wait.
void ta_cond_init(tacond_t *cond) {
    ta_sem_init(&cond->sem, 0);
}

// do nothing
void ta_cond_destroy(tacond_t *cond) {
}

void ta_wait(talock_t *mutex, tacond_t *cond) {
    ta_unlock(mutex);
    ta_sem_wait(&cond->sem);
    ta_lock(mutex);
}

void ta_signal(tacond_t *cond) {
    ta_sem_post(&cond->sem);
}