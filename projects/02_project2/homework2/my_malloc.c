#include "my_malloc.h"

// linked list
__thread Node * ll_head = NULL;
__thread Node * ll_tail = NULL;
size_t node_size = sizeof(Node);

// hw 2
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

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

void split(Node * curr, size_t size) {
    size_t remaining = curr->sum_size - size - sizeof(Node);
    if (remaining > sizeof(Node)) {
        // printf -= (sizeof(Node) + size);

        Node * remain = (Node *)((void *)curr + sizeof(Node) + size);
        remain->sum_size = remaining;
        remain->next = curr->next;
        remain->prev = curr->prev;

        // if curr is not head, link curr->prev to remain
        if (curr->prev) {
            curr->prev->next = remain;
        }
        // if curr is head, update new head
        else {
            ll_head = remain; 
        }
        // if curr is not tail, link curr->next to remain
        if (curr->next) {
            curr->next->prev = remain;
        }
        // if curr is tail, update new tail
        else {
            ll_tail = remain;
        }

        curr->sum_size = size + sizeof(Node);
    }
    // the remains are less than the metadata
    // no need to split
    else {
        // if curr is not head, link curr->prev to its next
        if (curr->prev) {
            curr->prev->next = curr->next;
        }
        // if curr is head, update new head with next node(or NULL)
        else {
            ll_head = curr->next;
        }
        // if curr is not tail, link the next one to its prev
        if (curr->next) {
            curr->next->prev = curr->prev;
        }
        // if curr is tail, update new tail with next node(or NULL)
        else {
            ll_tail = curr->prev;
        }
    }
    curr->prev = NULL;
    curr->next = NULL;
}

void merge(Node * curr) {
    // first check if can merge to the previous one
    // check if there is an existing prev
    if (curr->prev && (void *) curr->prev + curr->prev->sum_size == (void *) curr) {
        // now merge
        curr->prev->next = curr->next;
        curr->prev->sum_size += curr->sum_size;
        if (curr->next) {
            curr->next->prev = curr->prev;
        } else {
            ll_tail = curr->prev;
        }
        // update after merge
        curr = curr->prev; 
    }

    // now check if can merge the next one
    // check if there is an existing next
    if (curr->next && (void *) curr + curr->sum_size == (void *) curr->next) {
        // now merge
        curr->sum_size += curr->next->sum_size;
        curr->next = curr->next->next;
        if (curr->next) {
            curr->next->prev = curr;
        } else {
            ll_tail = curr;
        }
    }
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

void * bf_malloc(size_t size, int sync) {
    // printLL(ll_head);
    // if there is no enough
    Node * bf = checkBF(size);

    if (bf == NULL) {
        // printf("Didn't find the valid place!\n");
        void * ptr;
        if (sync == 0) {
            ptr = (void *)sbrk(size + sizeof(Node));
            if (ptr == (void *)-1) {
                return NULL; // sbrk failed
            }
        }
        else if (sync == 1) {
            pthread_mutex_lock(&lock);
            ptr = (void *)sbrk(size + sizeof(Node));
            if (ptr == (void *)-1) {
                return NULL; // sbrk failed
            }
            pthread_mutex_unlock(&lock);
        }

        // save the meta data
        Node * metadata = (Node *)ptr;
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

// lock version
void * ts_malloc_lock(size_t size) {
    pthread_mutex_lock(&lock);
    void * ptr = bf_malloc(size, 0);
    pthread_mutex_unlock(&lock);
    return ptr;
}

void ts_free_lock(void *ptr) {
    pthread_mutex_lock(&lock);
    bf_free(ptr);
    pthread_mutex_unlock(&lock);
}

// sync version
void * ts_malloc_nolock(size_t size) {
    void * ptr = bf_malloc(size, 1);
    return ptr;
}

void ts_free_nolock(void *ptr) {
    bf_free(ptr);
}
