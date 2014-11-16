#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include "threadsalive.h"

#define DATALEN 2
#define DURATION 3

tasem_t readersem;
tasem_t writersem;
talock_t wmutex;
talock_t rmutex;
int readerloc = 0;
int writerloc = 0;

int *data = NULL;
int datalen = 0;
int stop = 0;

void killerthr(void *arg)
{
    time_t now = time(NULL);
    time_t finish = now + DURATION;
    while (now < finish) {
        ta_yield();
        now = time(NULL);
    }
    stop = 1;
}

void reader(void *arg)
{
    int tid = (int)arg;
    int val = 0;
    while (!stop) {
        ta_sem_wait(&readersem);
        ta_lock(&rmutex);
        int loc = readerloc;
        readerloc = (readerloc+1) % datalen;
        ta_unlock(&rmutex);
        val = data[loc];
        ta_sem_post(&writersem);
        fprintf(stderr, "reader %d read location %d\n", tid, loc);

        if (random() % 2 == 0)
            ta_yield();
    }
}

void writer(void *arg)
{
    int tid = (int)arg;
    int val = 1000000;
    int writerloc = 0;
    while (!stop) {
        ta_sem_wait(&writersem);

        ta_lock(&wmutex);
        int loc = writerloc;
        writerloc = (writerloc+1) % datalen;
        ta_unlock(&wmutex);

        data[loc] = val++;
        ta_sem_post(&readersem);
        fprintf(stderr, "writer %d wrote location %d\n", tid, loc);

        if (random() % 2 == 0)
            ta_yield();
    }
}

int main(int argc, char **argv)
{
    printf("Tester for stage 2.  Uses semaphores and mutex locks.\n");

    srandom(time(NULL));

    ta_libinit();
    int i = 0;
    int nrw = 2;

    data = (int *)malloc(sizeof(int) * DATALEN);
    assert(data);
    memset(data, 0, sizeof(int)*DATALEN);
    datalen = DATALEN;

    ta_sem_init(&readersem, 0);
    ta_sem_init(&writersem, DATALEN);
    ta_lock_init(&rmutex);
    ta_lock_init(&wmutex);

    ta_create(killerthr, (void *)i);

    for (i = 0; i < nrw; i++) {
        ta_create(reader, (void *)i);
        ta_create(writer, (void *)i);
    }

    int rv = ta_waitall();
    assert(rv == 0);

    ta_sem_destroy(&readersem);
    ta_sem_destroy(&writersem);
    ta_lock_destroy(&rmutex);
    ta_lock_destroy(&wmutex);

    free(data);

    return 0;
}
