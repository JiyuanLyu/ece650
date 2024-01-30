#ifndef __MY_MALLOC_H__
#define __MY_MALLOC_H__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Node {
    // this size include the metadata size (the node size)
    size_t sum_size;
    struct Node *prev;
    struct Node *next;
} Node;
void printLL(Node *head);

void split(Node *curr, size_t size);
void merge(Node *curr);

// Best Fit malloc/free
void * bf_malloc(size_t size);
void bf_free(void *ptr);

// check function will return NULL if there is no fit
// it will return a pointer to the fitted Node if there is a fit
Node *checkBF(size_t size);

//Thread Safe malloc/free: locking version 
void * ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);

//Thread Safe malloc/free: non-locking version 
void * ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr);

#endif