#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "ymalloc.h"

#define NEW_ALLOC 0x1
#define RE_ACCLOC 0x2
#define FREED 0x3

#define exit_with_error(msg) do {perror(msg); exit(EXIT_FAILURE);} while(0)

typedef struct mem_block_header {
    size_t block_size;
    int free;
    int magic;
    struct mem_block_header *next;
} block_header_t;

#define HEADER_SIZE sizeof(block_header_t)
void *mem_pool = NULL;

block_header_t *get_space_from_os(size_t size) {
    block_header_t *block = sbrk(0);
    void *block_request = sbrk(size + HEADER_SIZE);
    if (block_request == (void *)-1) {
        return NULL;
    }

    // populate header data
    block->block_size = size;
    block->free = 0;
    block->next = NULL;
    block->magic = NEW_ALLOC;

    return block;
}

block_header_t *find_free_block_first_fit(block_header_t **last, size_t size) {
    block_header_t *curr = mem_pool;
    while (curr != NULL && !(curr->free == 1 && curr->block_size >= size)) {
        *last = curr;
        curr = curr->next;
    }
    return curr;
}

block_header_t *find_free_block_best_fit(block_header_t **last, size_t size) {
    block_header_t *curr = mem_pool;
    block_header_t *block = NULL;
    while (curr != NULL) {
        *last = curr;
        if (curr->free == 1 && curr->block_size >= size && (block == NULL || curr->block_size < block->block_size)) {
            block = curr;
        }
        curr = curr->next;
    }
    return block;
}

block_header_t *find_free_block(block_header_t **last, size_t size, placement_t placement) {
    block_header_t *block = NULL;
    switch(placement) {
        case FIRST_FIT:
            block = find_free_block_first_fit(last, size);
            break;
        case BEST_FIT:
            block = find_free_block_best_fit(last, size);
            break;
    }
    return block;
}


void add_block_to_pool(block_header_t *block) {
    if (mem_pool == NULL) {
        mem_pool = block;
        return;
    }
    block_header_t *curr = mem_pool;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = block;
    return;
}

void *ymalloc(size_t size, placement_t placement) {
    if (size <= 0) {
        return NULL;
    }
    block_header_t *block = NULL;

    // first allocation
    if (mem_pool == NULL) {
        block = get_space_from_os(size);
        if (block == NULL) {
            return NULL;
        }
        mem_pool = block;
        return (void *)(block + 1);
    }
    block_header_t *last = mem_pool;
    block = find_free_block(&last, size, placement);
    if (block != NULL) {
        //printf ("found free block: %ld\n", block);
        block->free = 0;
        block->magic = RE_ACCLOC;
        return (void *)(block + 1);
    }
    //printf("Not found free block\n");
    block = get_space_from_os(size);
    if (block == NULL) {
        return NULL;
    }
    //printf("got block from OS: %ld\n", block);
    last->next = block;
    //printf("pool header %ld\n", mem_pool);
    return (void *)(block + 1);
}

block_header_t *get_header_from_data_block(void *data_block) {
    return (block_header_t *)data_block - 1;
}

void free(void *data_block) {
    if (data_block == NULL) {
        return;
    }
    block_header_t *block = get_header_from_data_block(data_block);
    if (block->free == 1) {
        return;
    }
    if (!(block->magic == NEW_ALLOC || block->magic == RE_ACCLOC)) {
        return;
    }
    block->free = 1;
    block->magic = FREED;
}
