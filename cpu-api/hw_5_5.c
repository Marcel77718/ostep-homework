#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main (int argc, char *argv[]) {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        printf("pid vom child:%d\n", (int) getpid());
        
        char *myargs[2];
        myargs[0] = strdup("/bin/ls");
        myargs[1] = NULL;
        execvp(myargs[0], myargs);
        printf("should't be printed\n");
    } else {
        int rc_wait = wait(NULL);
        printf("\nFertig.\n");
        printf("wait() returns: %d\n", rc_wait);
        printf("'%d' ist die PID vom Child, während die PID vom parent: %d ist.\n", rc_wait,(int) getpid());
    }
}