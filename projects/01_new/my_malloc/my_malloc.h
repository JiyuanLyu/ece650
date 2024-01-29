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

// First Fit malloc/free
void *ff_malloc(size_t size);
void ff_free(void *ptr);
// Best Fit malloc/free
void * bf_malloc(size_t size);
void bf_free(void *ptr);

// check function will return NULL if there is no fit
// it will return a pointer to the fitted Node if there is a fit
Node *checkFF(size_t size);
Node *checkBF(size_t size);

unsigned long get_data_segment_size();            // in bytes
unsigned long get_data_segment_free_space_size(); // in byte

#endif