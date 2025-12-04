#include <stdlib.h>
#include <pthread.h>
#include "common_threads.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int x;

void *worker(void* arg) {
    while (1) {
        Pthread_mutex_lock(&lock);
        if (x == 20) { Pthread_mutex_unlock(&lock); break; }
        else { x++;}
        Pthread_mutex_unlock(&lock);
    }
    return NULL;
}
    
int main(int argc, char* argv[]) {
    x = 0;
    printf("x ist am start: %d\n", x);
    pthread_t t1, t2, t3;
    Pthread_create(&t1, NULL, worker, NULL);
    Pthread_create(&t2, NULL, worker, NULL);
    Pthread_create(&t3, NULL, worker, NULL);
    Pthread_join(t1, NULL);
    Pthread_join(t2, NULL);
    Pthread_join(t3, NULL);
    printf("x ist am ende: %d\n", x);
    return 0;
}