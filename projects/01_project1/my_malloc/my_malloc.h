#ifndef __MY_MALLOC_H__
#define __MY_MALLOC_H__

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    // this size include the metadata size (the node size)
    size_t sum_size;
    struct Node * prev;
    struct Node * next;
} Node ;

void split(Node * curr, size_t size);
// void merge(Node * curr, Node * insert);

//First Fit malloc/free
void * ff_malloc(size_t size);
void ff_free(void *ptr);
//Best Fit malloc/free
//void * bf_malloc(size_t size);
//void bf_free(void *ptr);

// check function will return 0 if there is no fit
// it will return 1 if there is fit
Node * checkFF(Node * head, size_t size);
Node * checkBF(Node * head, size_t size);

unsigned long get_data_segment_size(); //in bytes
unsigned long get_data_segment_free_space_size(); //in byte

#endif