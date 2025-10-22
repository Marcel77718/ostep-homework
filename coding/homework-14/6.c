#include <stdlib.h>
#include <stdio.h>

int main (void) {
    int *data = malloc(100 * sizeof(int));
    free(data);
    printf("%d\n", data[0]);
    return 0;
}