#include <stdlib.h>
#include <stdio.h>

int main(void) {
    int *data = malloc(100 * sizeof(int));
    int *mid = data + 50;
    free(mid);
    return 0;
}
