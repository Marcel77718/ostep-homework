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
        char *myargs[2];
        myargs[0] = strdup("/bin/ls");
        myargs[1] = NULL;
        execvp(myargs[0], myargs);
        // execl müsste man als liste durch komma getrennt übergeben = execl("/bin/ls", "ls", "-l", NULL)
        // ecec mit v = array übergabe
        // mit p = argumente suchen im systempfad
        // mit e = umgebungsvariablen des prozess definieren
        printf("should't be printed\n");
    } else {
        int rc_wait = wait(NULL);
        printf("\nFertig.\n");
    }
}