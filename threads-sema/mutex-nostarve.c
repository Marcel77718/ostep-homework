#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "common_threads.h"

//
// Here, you have to write (almost) ALL the code. Oh no!
// How can you show that a thread does not starve
// when attempting to acquire this mutex you build?
//

typedef struct __ns_mutex_t {
    // protects ticket/turn
    sem_t lock;

    // ticket dispenser + current turn
    int ticket;
    int turn;

    // array of per-ticket semaphores (simple bounded test harness)
    // NOTE: this test assumes a maximum number of total acquisitions.
    sem_t *q;
    int qsize;
} ns_mutex_t;

void ns_mutex_init(ns_mutex_t *m, int qsize) {
    sem_init(&m->lock, 0, 1);
    m->ticket = 0;
    m->turn   = 0;

    m->qsize = qsize;
    m->q = (sem_t *)malloc(sizeof(sem_t) * qsize);
    assert(m->q != NULL);

    for (int i = 0; i < qsize; i++) {
        sem_init(&m->q[i], 0, 0);
    }

    // allow ticket 0 to pass immediately
    sem_post(&m->q[0]);
}

void ns_mutex_acquire(ns_mutex_t *m) {
    sem_wait(&m->lock);
    int my = m->ticket++;
    sem_post(&m->lock);

    // wait for your turn
    sem_wait(&m->q[my]);
    // now you own the mutex (because turns are serialized)
}

void ns_mutex_release(ns_mutex_t *m) {
    sem_wait(&m->lock);
    int next = ++m->turn;
    sem_post(&m->lock);

    // wake next ticket holder
    sem_post(&m->q[next]);
}

// ------------------- test harness -------------------

typedef struct {
    int tid;
    int loops;
} tinfo_t;

ns_mutex_t m;
volatile int shared = 0;

void *worker(void *arg) {
    tinfo_t *t = (tinfo_t *)arg;
    for (int i = 0; i < t->loops; i++) {
        ns_mutex_acquire(&m);

        // critical section
        int tmp = shared;
        tmp++;
        shared = tmp;

        ns_mutex_release(&m);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    assert(argc == 3);
    int nthreads = atoi(argv[1]);
    int loops    = atoi(argv[2]);
    assert(nthreads > 0 && loops > 0);

    // total number of lock acquisitions across all threads
    int total = nthreads * loops;

    printf("parent: begin\n");

    ns_mutex_init(&m, total + 1);

    pthread_t p[nthreads];
    tinfo_t t[nthreads];

    for (int i = 0; i < nthreads; i++) {
        t[i].tid = i;
        t[i].loops = loops;
        Pthread_create(&p[i], NULL, worker, &t[i]);
    }

    for (int i = 0; i < nthreads; i++) {
        Pthread_join(p[i], NULL);
    }

    printf("parent: end (shared=%d, expected=%d)\n", shared, total);
    return 0;
}