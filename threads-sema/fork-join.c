#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "common_threads.h"
#include <semaphore.h>

sem_t s; 

void *child(void *arg) {
    printf("child\n");
    sem_post(&s);
    sleep(1);
    // use semaphore here
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p;
    printf("parent: begin\n");
    // init semaphore here
    sem_init(&s, 0, 0);
    Pthread_create(&p, NULL, child, NULL);
    // use semaphore here
    sem_wait(&s);
    printf("parent: end\n");
    return 0;
}

