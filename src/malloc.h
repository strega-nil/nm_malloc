#ifndef _MALLOC_H
#define _MALLOC_H
#include <unistd.h>

void *nm_malloc(size_t size);
void nm_free(void *ptr);

#endif
