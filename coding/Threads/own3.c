#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "common_threads.h"

typedef struct __synchronizer_t {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int done;
    int counter;
} synchronizer_t;

static synchronizer_t s;

void *initilazer(synchronizer_t *s) {
    pthread_mutex_init(&s->lock, NULL);
    pthread_cond_init(&s->cond, NULL);
    s->done = 0;
    s->counter = 0;
}

void *waiter(synchronizer_t *s) {
    Pthread_mutex_lock(&s->lock);
    while (s->done == 0) {
        Pthread_cond_wait(&s->cond, &s->lock);
    }
    Pthread_mutex_unlock(&s->lock);
}

/* assumes caller already holds s->lock */
static void set_and_signal(synchronizer_t *s) {
    s->done = 1;
    Pthread_cond_signal(&s->cond);
}

void *worker(void *arg) {
    synchronizer_t *s = (synchronizer_t *) arg;
    Pthread_mutex_lock(&s->lock);
    while (s->counter < 20) {
        s->counter++;
    }
    set_and_signal(s);
    Pthread_mutex_unlock(&s->lock);
    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t t1;
    initilazer(&s);
    Pthread_create(&t1, NULL, worker, &s);
    waiter(&s);
    printf("Die zahl am ende ist: %d\n", s.counter);
    return 0;
}