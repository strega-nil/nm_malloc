#define _BSD_SOURCE

#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/mman.h>

#include "malloc.h"

typedef struct malloc_ptr {
    size_t size;
    // If unused, the first 4/8 bytes of this are a pointer to the next
    // unused malloc_ptr
    void *buffer[];
} malloc_ptr;

#define full_size(mptr) (mptr->size + sizeof *mptr)

static const size_t BIG_PTR_SIZE = 128 * 1024;
static malloc_ptr UNUSED_LIST = { // List of sorted (by size) malloc_ptrs
    .size = 1,
    .buffer = {NULL},
};

void *malloc_small(size_t size);
void *malloc_big(size_t size);
void free_small(malloc_ptr *mptr);
void free_big(malloc_ptr *mptr);

void *nm_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    // The buffer's size should be aligned on sizeof(malloc_ptr)
    size = ((size - 1) / sizeof(malloc_ptr) + 1) * sizeof(malloc_ptr);

    if (size < BIG_PTR_SIZE) {
        return malloc_small(size);
    } else {
        return malloc_big(size);
    }
}

void nm_free(void *ptr) {
    if(ptr == NULL) {
        return;
    }

    malloc_ptr *mptr = (malloc_ptr *)ptr - 1;

    if (mptr->size < BIG_PTR_SIZE) {
        free_small(mptr);
    } else {
        free_big(mptr);
    }
}

void *malloc_big(size_t size) {
    malloc_ptr *mptr = mmap(NULL, size + sizeof *mptr, PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANON, -1, 0);

    if (mptr == (void *) -1) {
        return NULL;
    }

    mptr->size = size;

    return &(mptr->buffer);
}

void free_big(malloc_ptr *mptr) {
    munmap(mptr, mptr->size + sizeof *mptr);
}

void *malloc_small(size_t size) {
    malloc_ptr *prev = &UNUSED_LIST;
    malloc_ptr *curr = (malloc_ptr *)prev->buffer[0];
    while (curr != NULL) {
        if (curr->size < size) {
            prev = curr;
            curr = (malloc_ptr *)prev->buffer[0];
        } else if ((size + sizeof *curr) > (2 * full_size(curr))) {
            // Takes curr out of the list, in case fullsize(new ptr) < 2 *
            // fullsize(old ptr)
            prev->buffer[0] = curr->buffer[0];
            return &(curr->buffer);
        } else {
            // technically this should be
            // `curr->size + sizeof *curr - size - sizeof *curr - sizeof *curr`
            // but we can simplify
            size_t new_size = curr->size - size - sizeof *curr;
            malloc_ptr *new_curr = (void *)((int8_t *)&(curr->buffer) + size);
            prev->buffer[0] = new_curr;
            new_curr->buffer[0] = curr->buffer[0];

            curr->size = size;
            new_curr->size = new_size;

            return &(curr->buffer);
        }
    }

    curr = sbrk(sizeof *curr + size);
    if (curr == (void *) -1) {
        return NULL; // sbrk failure
    }

    curr->size = size;

    return &(curr->buffer);
}

void free_small(malloc_ptr *mptr) {
    malloc_ptr *prev = &UNUSED_LIST;
    malloc_ptr *curr = (malloc_ptr *)prev->buffer[0];
    while (curr != NULL) {
        if (curr->size < mptr->size) {
            prev = curr;
            curr = prev->buffer[0];
        } else {
            prev->buffer[0] = mptr;
            mptr->buffer[0] = curr;
            return;
        }
    }
    prev->buffer[0] = mptr;
    mptr->buffer[0] = NULL;
}
