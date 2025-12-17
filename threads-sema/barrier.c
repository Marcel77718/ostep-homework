#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#include "common_threads.h"

typedef struct __barrier_t {
    int num_threads;
    int counter;
    pthread_mutex_t lock;

    // two semaphores ("turnstiles")
    sem_t s1;
    sem_t s2;
} barrier_t;

barrier_t b;

void barrier_init(barrier_t *b, int num_threads) {
    b->num_threads = num_threads;
    b->counter = 0;

    pthread_mutex_init(&b->lock, NULL);

    // first gate closed, second gate open
    sem_init(&b->s1, 0, 0);
    sem_init(&b->s2, 0, 1);
}

void barrier(barrier_t *b) {
    // phase 1: arrive
    pthread_mutex_lock(&b->lock);
    b->counter++;
    if (b->counter == b->num_threads) { //Wenn letzter Thread rein kommt macht er Tür 1 auf und Tür 2 zu
        // last one closes s2 and opens s1
        sem_wait(&b->s2);
        sem_post(&b->s1);
    }
    pthread_mutex_unlock(&b->lock);

    // pass through first gate (baton passing)
    sem_wait(&b->s1);
    sem_post(&b->s1);

    // phase 2: depart
    pthread_mutex_lock(&b->lock);
    b->counter--;
    if (b->counter == 0) {
        // last one closes s1 and opens s2
        sem_wait(&b->s1);
        sem_post(&b->s2);
    }
    pthread_mutex_unlock(&b->lock);

    // pass through second gate (baton passing)
    sem_wait(&b->s2);
    sem_post(&b->s2);
}

typedef struct __tinfo_t {
    int thread_id;
} tinfo_t;

void *child(void *arg) {
    tinfo_t *t = (tinfo_t *) arg;
    printf("child %d: before\n", t->thread_id);
    barrier(&b);
    printf("child %d: after\n", t->thread_id);
    return NULL;
}

int main(int argc, char *argv[]) {
    assert(argc == 2);
    int num_threads = atoi(argv[1]);
    assert(num_threads > 0);

    pthread_t p[num_threads];
    tinfo_t t[num_threads];

    printf("parent: begin\n");
    barrier_init(&b, num_threads);

    for (int i = 0; i < num_threads; i++) {
        t[i].thread_id = i;
        Pthread_create(&p[i], NULL, child, &t[i]);
    }

    for (int i = 0; i < num_threads; i++)
        Pthread_join(p[i], NULL);

    printf("parent: end\n");
    return 0;
}
