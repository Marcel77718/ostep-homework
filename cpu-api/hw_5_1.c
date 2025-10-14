#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int x;

int main (void) {
    x = 100;
    printf("x vor fork: %d\n", x);
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed");
        exit(1);
    } else if (rc == 0) {
        printf("x in child before changed: %d\n", x);
        x = 5;
        printf("x in child: %d\n", x);
    } else {
        //wait(NULL);
        printf("x in parent: %d\n", x);
    }
    return 0;
}