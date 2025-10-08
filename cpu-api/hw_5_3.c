#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

/*
int main (int argc, char *argv[]) {
    close(STDOUT_FILENO);
    char *msg;
    int fd = open("./hw_5_3.output", O_CREAT | O_WRONLY | O_TRUNC,
        S_IRWXU);

    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if(rc == 0) {
        msg = "Hello.\n";
        if (fd < 0) {
            fprintf(stderr, "Error opening file\n");
            exit(1);
        } else {
            write(fd, msg, strlen(msg));
        }
    } else {
        msg = "Goodbye.\n";
        if (fd < 0) {
        fprintf(stderr, "Error opening file\n");
        exit(1);
        } else {
            // int rc_wait = wait(NULL);
            write(fd, msg, strlen(msg));
        }
    }
}
*/

int main (int argc, char *argv[]) {
    char *msg;
    int rc = fork();

    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        msg = "Hello.\n";
        printf("%s", msg);
    } else {
        msg = "Goodbye.\n";
        //int rc_wait = wait(NULL);
        // ansosnten mit sleep ??
        sleep(1);
        printf("%s", msg);
    }
}