#include <stdio.h>

#include "common_threads.h"

int balance = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 

void* worker(void* arg) {
    Pthread_mutex_lock(&lock);
    printf("ich bin im worker thread und habe for balance++ die zahl: %d\n", balance);
    balance++; // unprotected access 
    Pthread_mutex_unlock(&lock);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p;
    Pthread_create(&p, NULL, worker, NULL);
    Pthread_mutex_lock(&lock);
    printf("ich bin im main thread und habe for balance++ die zahl: %d\n", balance);
    balance++; // unprotected access
    Pthread_mutex_unlock(&lock);
    Pthread_join(p, NULL);
    printf("%d\n", balance);
    return 0;
}
