#include <stdlib.h>
#include <pthread.h>
#include "common_threads.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int x;
int done;



void* watcher(void* arg) {
    Pthread_mutex_lock(&lock);
    while (done == 0)
    Pthread_cond_wait(&cond, &lock);
    Pthread_mutex_unlock(&lock);
    return NULL;
}


void *worker(void* arg) {
    while (1) {
        Pthread_mutex_lock(&lock);
        if (x == 20) {
            done = 1;
            Pthread_mutex_unlock(&lock);
            Pthread_cond_signal(&cond);
            break; 
        } else { x++;}
        Pthread_mutex_unlock(&lock);
    }
    return NULL;
}
    
int main(int argc, char* argv[]) {
    x = 0;
    done = 0;
    printf("x ist am start: %d\n", x);
    pthread_t t1, t2, t3;
    Pthread_create(&t1, NULL, worker, NULL);
    Pthread_create(&t2, NULL, worker, NULL);
    Pthread_create(&t3, NULL, watcher, NULL);

    Pthread_join(t1, NULL);
    Pthread_join(t2, NULL);
    Pthread_join(t3, NULL);

    printf("x ist am ende: %d\n", x);
    return 0;
}