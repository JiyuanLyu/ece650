#include "my_malloc.h"

// linked list
Node * ll_head = NULL;

// implement linked list check function here
Node * checkFF(Node * head, size_t size) {
    Node * curr = head;
    if (curr == NULL) {
        return NULL;
    }
    while (curr != NULL) {
        if (curr->sum_size - sizeof(Node) >= size) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void split(Node * curr, size_t size) {
    // Node * remain = (Node *)((char *)curr + sizeof(Node) + size);
    // remain->prev = curr->prev;
    // remain->next = curr->next;

    size_t remaining = curr->sum_size - size - sizeof(Node);
    // if need to split
    if (remaining > sizeof(Node)) {
        Node * remain = (Node *)((void *)curr + sizeof(Node) + size);
        remain->sum_size = remaining;
        remain->next = curr->next;
        remain->prev = curr->prev;
        curr->prev->next = remain;
        if (curr->next != NULL) {
            curr->next->prev = remain;
        }
    }
    // the remains are less than the metadata
    // no need to split
    else {
        curr->prev->next = curr->next;
        curr->next->prev = curr->prev;
    }
    curr->sum_size = size + sizeof(Node);
    curr->prev = NULL;
    curr->next = NULL;
}

void * ff_malloc(size_t size) {
    // if there is no enough
    Node * ff = checkFF(ll_head, size);
    if (ff == NULL) {
        void * ptr = sbrk(size + sizeof(Node));
        if (ptr == (void *) -1) {
            return NULL; // sbrk failed
        }
        // save the meta data
        Node * metadata = (Node * )ptr;
        metadata->next = NULL;
        metadata->prev = NULL;
        metadata->sum_size = sizeof(Node) + size;
        // return the ptr after the meta data
        return ptr + sizeof(Node);
    }
    // means that there is a node ff that could be used for the malloc
    // check if it need to split
    // and then return the address at the end of the metadata
    else {
        split(ff, size);
        return (void *) ff + sizeof(Node);
    }
}

void ff_free(void * ptr) {
    Node * metadata = (Node * ) (ptr - sizeof(Node));

    Node * curr = ll_head;
    // if at the head
    if (curr == NULL) {
        curr->next = metadata;
        metadata->prev = curr;
        return;
    }

    while (curr < metadata) {
        if (curr->next != NULL) {
            curr = curr->next;
        }
        // if at the tail
        else {
            curr->next = metadata;
            metadata->prev = curr;
            return;
        }
    }
    if (metadata + metadata->sum_size == curr) {
        // now merge with the next
        metadata->sum_size += curr->sum_size;
        metadata->prev = curr->prev;
        metadata->next = curr->next;
        metadata->prev->next = metadata;
        metadata->next->prev = metadata;
    }
    else {
        metadata->prev = curr->prev;
        metadata->next = curr;
        metadata->prev->next = metadata;
        metadata->next->prev = metadata;
    }
}


unsigned long get_data_segment_size() {
    return 0;
}

unsigned long get_data_segment_free_space_size() {
    return 0;
}