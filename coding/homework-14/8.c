#include <stdlib.h>
#include <stdio.h>

typedef struct {
    int *data;
    int size;
} Vector;

void vector_init(Vector *v) {
    v->data = NULL;
    v->size = 0;
}

void vector_add(Vector *v, int value) {
    v->size++;
    v->data = realloc(v->data, v->size * sizeof(int));
    v->data[v->size - 1] = value;
}

void vector_free(Vector *v) {
    free(v->data);
}

int main(void) {
    Vector v;
    vector_init(&v);

    vector_add(&v, 10);
    vector_add(&v, 20);
    vector_add(&v, 30);

    for (int i = 0; i < v.size; i++)
        printf("%d ", v.data[i]);
    printf("\n");

    vector_free(&v);
    return 0;
}
