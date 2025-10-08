#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int fd[2];
    if (pipe(fd) == -1) {
        fprintf(stderr, "pipe failed.\n");
        exit(1);
    }

    pid_t rc1 = fork();
    if (rc1 < 0) {
        fprintf(stderr, "1st fork failed");
        exit(1);
    } else if (rc1 == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        execlp("ls", "ls", "-1", (char *)NULL);
        fprintf(stderr, "exec producer");
        exit(1);
    }

    pid_t rc2 = fork();
    if (rc2 < 0) {
        fprintf(stderr, "2nd fork failed");
        exit(1);
    } else if (rc2 == 0) {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);

        execlp("wc", "wc", "-l", (char *)NULL);
        fprintf(stderr, "exec consumer");
        exit(1);
    } else {
        close(fd[0]);
        close(fd[1]);
        waitpid(rc1, NULL, 0);
        waitpid(rc2, NULL, 0);
        return 0;
    }
}