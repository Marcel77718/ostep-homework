#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main (int argc, char *argv[]) {
    close(STDOUT_FILENO);
    char *msg = "Das ist vor dem forken der Inhalt.\n";

    int fd = open("./hw_5_2.output", O_CREAT | O_WRONLY | O_TRUNC,
        S_IRWXU);

    write(fd, msg, strlen(msg));

    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if(rc == 0) {
        msg = "Das ist im Child der Inhalt.\n";
        if (fd < 0) {
            fprintf(stderr, "Error opening file");
            exit(1);
        } else {
            write(fd, msg, strlen(msg));
        }
    } else {
        msg = "Das ist im Parent der Inhalt.\n";
        if (fd < 0) {
        fprintf(stderr, "Error opening file");
        exit(1);
        } else {
            int rc_wait = wait(NULL);
            write(fd, msg, strlen(msg));
        }
    }
}