#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>


int main (int argc, char *argv[]) {
    char *msg = "printf before child.\n";
    printf("%s", msg);
    int rc = fork();

    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        msg = "printf before STDOUT_FILENO\n";
        close(STDOUT_FILENO);
        printf("%s", msg);
    } else {
        msg = "print von parent\n";
        int rc_wait = wait(NULL);
        printf("%s", msg);
    }
}
//* egal ob wait oder nicht, msg in child wird nicht geprinted