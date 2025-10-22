#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Keine Argumente, bitte Anzahl an MB angeben, die im Speicher reserviert werden sollen.\n");
        exit(1);
    } else if (argc > 3) {
        fprintf(stderr, "Zu viele Argumente.\n");
        exit(1);
    }

    int pid = getpid();
    printf("pid ist: %d\n", pid);

    size_t b = (size_t)atoi(argv[1]) * 1024 * 1024;

    int t = -1;
    if (argc == 3) {
        t = atoi(argv[2]);
        if (t <= 0) {
            fprintf(stderr, "Zeit muss positiv sein.\n");
            exit(1);
        }
    }

    char *array = malloc(b);
    if (array == NULL) {
        fprintf(stderr, "malloc fehlgeschlagen\n");
        exit(1);
    }

    printf("Reserviere %zu MB Speicher...\n", b / (1024 * 1024));
    printf("Starte Zugriff...\n");

    time_t start = time(NULL);

    while (1) {
        for (size_t i = 0; i < b; i++) {
            array[i]++;
        }
        if (t > 0 && difftime(time(NULL), start) >= t) {
            break;
        }
    }

    printf("Fertig.\n");
    free(array);
    sleep(4);
    return 0;
}
