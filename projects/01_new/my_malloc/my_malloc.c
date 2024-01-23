#include "my_malloc.h"

// linked list
Node *ll_head = NULL;
Node *ll_tail = NULL;
unsigned long heap_size = 0;
//unsigned long free_size = 0;
size_t node_size = sizeof(Node);
// use this function to check my free mem linked list
void printLL(Node * head) {
    if (head == NULL) {
        // printf("The linked list is empty!\n");
        return;
    }

    printf("The head node sum size is %ld\n", head->sum_size);
    Node * curr = head;
    int count = 0;
    while (curr != ll_tail) {
        curr = curr->next;
        count++;
        printf("The %d node sum size is %ld\n", count, curr->sum_size);
    }
    printf("The tail node sum size is %ld\n", ll_tail->sum_size);
    printf("Reach the end of linked list!\n");
}

// implement linked list check function here
Node * checkFF(size_t size) {
    if (ll_head == NULL) {
        return NULL;
    }
    Node * curr = ll_head;
    while (curr != NULL) {
        if (curr->sum_size - sizeof(Node) >= size) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void split(Node * curr, size_t size) {
    size_t remaining = curr->sum_size - size - sizeof(Node);
    if (remaining > sizeof(Node)) {
        // printf -= (sizeof(Node) + size);

        Node * remain = (Node *)((void *)curr + sizeof(Node) + size);
        remain->sum_size = remaining;
        remain->next = curr->next;
        remain->prev = curr->prev;

        // if (curr == ll_head && curr == ll_tail) {
        //     ll_head = remain;
        //     ll_tail = remain;
        // }
        // else if (curr == ll_head) {
        //     curr->next->prev = remain;
        //     ll_head = remain;
        // }
        // else if (curr == ll_tail) {
        //     curr->prev->next = remain;
        //     ll_tail = remain;
        // }
        // else {
        //     curr->prev->next = remain;
        //     curr->next->prev = remain;
        // }
        if (curr->prev) {
            curr->prev->next = remain;
        }
        else {
            ll_head = remain; // Update head if curr is the first node
        }

        if (curr->next) {
            curr->next->prev = remain;
        }
        else {
            ll_tail = remain; // Update tail if the current block was the last one
        }

        curr->sum_size = size + sizeof(Node);

        // curr->sum_size -= (size + sizeof(Node));
    }
    // the remains are less than the metadata
    // no need to split
    else {
        // printf -= curr->sum_size;
        // if curr is both the head and tail
        // if (curr == ll_head && curr == ll_tail) {
        //     ll_head = NULL;
        //     ll_tail = NULL;
        // }
        // // if curr is the head
        // else if (curr == ll_head) {
        //     curr->next->prev = NULL;
        //     ll_head = curr->next;
        // }
        // else if (curr == ll_tail) {
        //     curr->prev->next = NULL;
        //     ll_tail = curr->prev;
        // }
        // // if curr is in the middle
        // else {
        //     curr->prev->next = curr->next;
        //     curr->next->prev = curr->prev;
        // }
        // Update links of neighboring nodes
        if (curr->prev) {
            curr->prev->next = curr->next;
        } else {
            ll_head = curr->next; // Update head if curr is the first node
        }

        if (curr->next) {
            curr->next->prev = curr->prev;
        } else {
            ll_tail = curr->prev; // Update tail if curr is the last node
        }
    }
    curr->prev = NULL;
    curr->next = NULL;
}


void *ff_malloc(size_t size) {
    // printLL(ll_head);
    // if there is no enough
    Node * ff = checkFF(size);
    
    if (ff == NULL) {
        // printf("Didn't find the valid place!\n");
        void * ptr = (void *) sbrk(size + sizeof(Node));
        if (ptr == (void *) -1) {
            return NULL; // sbrk failed
        }
        heap_size += (size + sizeof(Node));
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
        // printf("The valid space is %ld and malloc need %ld\n", ff->sum_size, size+sizeof(Node));
        split(ff, size);
        // printf("now check the ll after spliting\n");
        // printLL(ll_head);
        return (void *)ff + sizeof(Node);
    }
}

// void merge(Node * curr) {
//     // first check if can merge to the previous one
//     // if there is an existing prev
//     if (curr != ll_head && (void *) curr->prev + curr->prev->sum_size == (void *) curr) {
//         // now merge
//         curr->prev->next = curr->next;
//         curr->prev->sum_size += curr->sum_size;
//         if (curr != ll_tail) {
//             curr->next->prev = curr->prev;
//         }
//         else {
//             ll_tail = curr->prev;
//         }
//         // update after merge
//         curr = curr->prev;
//     }

//     // now check if can merge the next one
//     // if there is an existing next
//     if (curr != ll_tail && (void *) curr + curr->sum_size == (void *) curr->next) {
//         // now merge
//         curr->sum_size += curr->next->sum_size;
//         if (curr->next != ll_tail) {
//             curr->next->next->prev = curr;
//         }
//         else {
//             ll_tail = curr;
//         }
//         curr->next = curr->next->next;
//     }
// }

void merge(Node *curr) {
    // first check if can merge to the previous one
    // if there is an existing prev
    if (curr->prev && (void *)curr->prev + curr->prev->sum_size == (void *)curr) {
        curr->prev->sum_size += curr->sum_size; // Merge with previous
        curr->prev->next = curr->next;
        if (curr->next) {
            curr->next->prev = curr->prev;
        } else {
            ll_tail = curr->prev; // Update tail if necessary
        }
        curr = curr->prev; // Update current pointer to the merged block
    }

    // Check if the current block can be merged with the next block
    if (curr->next && (void *)curr + curr->sum_size == (void *)curr->next) {
        curr->sum_size += curr->next->sum_size; // Merge with next
        curr->next = curr->next->next;
        if (curr->next) {
            curr->next->prev = curr;
        } else {
            ll_tail = curr; // Update tail if necessary
        }
    }
}


void ff_free(void * ptr) {
    // if free NULL
    if (ptr == NULL) {
        // printf("The stack is not ff_malloc!\n");
        return;
    }

    Node * metadata = (Node *) (ptr - sizeof(Node));
    metadata->next = NULL;
    metadata->prev = NULL;

    // if empty linked list, insert in the first place
    if (ll_head == NULL) {
        ll_head = metadata;
        ll_tail = metadata;
        // printLL(ll_head);
        return;
    }

    // find where to insert the new node
    Node * curr = ll_head;
    // if there is only one node in the ll
    //if (curr->prev == NULL && curr->next == NULL) {
    if (ll_head == ll_tail) {
        if (curr < metadata) {
            // metadata is the last one
            curr->next = metadata;
            metadata->prev = curr;
            ll_head = curr;
            ll_tail = metadata;
        }
        else {
            // metadata is the first one
            curr->prev = metadata;
            metadata->next = curr;
            ll_head = metadata;
            ll_tail = curr;
        }
    }
    // if there are more than 1 node
    else {
        // printf("insert into ll\n");
        while (curr < metadata && curr != ll_tail) {
            curr = curr->next;
        }

        // insert before the head (llhead > metadata)
        if (curr == ll_head) {
            metadata->next = curr;
            curr->prev = metadata;
            ll_head = metadata;
        }
        // insert after tail
        else if (curr == ll_tail && curr < metadata) {
            metadata->prev = curr;
            curr->next = metadata;
            ll_tail = metadata;
        }
        // insert in the middle
        else {
            metadata->prev = curr->prev;
            metadata->next = curr;
            metadata->prev->next = metadata;
            metadata->next->prev = metadata;
        }
    }
    // check if it can merge into others
    // printLL(ll_head);
    merge(metadata);
    // printLL(ll_head);
}

Node * checkBF(size_t size) {
    if (ll_head == NULL) {
        return NULL;
    }
    Node * curr = ll_head;
    Node * bf = NULL;
    while (curr != NULL) {
        if (curr->sum_size - sizeof(Node) == size) {
            return curr;
        }
        else if (curr->sum_size - sizeof(Node) > size && (bf == NULL || bf->sum_size > curr->sum_size)) {
            bf = curr;
        }
        curr = curr->next;
    }
    return bf;
}

void * bf_malloc(size_t size) {
    // printLL(ll_head);
    // if there is no enough
    Node * bf = checkBF(size);

    if (bf == NULL) {
        // printf("Didn't find the valid place!\n");
        void *ptr = (void *)sbrk(size + sizeof(Node));
        if (ptr == (void *)-1) {
            return NULL; // sbrk failed
        }
        heap_size += (size + sizeof(Node));
        // save the meta data
        Node *metadata = (Node *)ptr;
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
        // printf("The valid space is %ld and malloc need %ld\n", ff->sum_size, size+sizeof(Node));
        split(bf, size);
        // printf("now check the ll after spliting\n");
        // printLL(ll_head);
        return (void *)bf + sizeof(Node);
    }
}

void bf_free(void * ptr) {
    ff_free(ptr);
}


unsigned long get_data_segment_size() {
    return heap_size;
}

unsigned long get_data_segment_free_space_size() {
    Node * curr = ll_head;
    unsigned long free_size = 0;
    while (curr != NULL) {
        free_size += curr->sum_size;
        curr = curr->next;
    }
    return free_size;
}