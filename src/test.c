#include "malloc.h"
#include <stdio.h>

int main(void) {
    int *i = nm_malloc(sizeof *i * 10);
    printf("%p\n", (void *)i);
    i[3] = 3;
    printf("%d\n", i[3]);
    nm_free(i);

    i = nm_malloc(sizeof *i * 30);
    printf("%p\n", (void *)i);
    i[29] = 42;
    printf("%d\n", i[29]);

    char *c = nm_malloc(10);
    char *c2 = nm_malloc(10);
    printf("%p\n%p\n", (void *)c, (void *)c2);
    c[9] = 'h';
    c2[9] = 'i';
    printf("%c\n%c\n", c[9], c2[9]);
    nm_free(c);
    nm_free(c2);
    nm_free(i);

    c = nm_malloc(60);
    printf("%p\n", (void *)c);
    c = "hello, world!";
    printf("%s\n", c);
    nm_free(c);
}
