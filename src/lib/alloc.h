#ifndef ALLOC_H_
#define ALLOC_H_

#include <errno.h>
#include <mem.h>
#include <assert.h>

#define MEMORY_CAPACITY 65536

struct block_meta {
  size_t size;
  struct block_meta *next;
  int free;
  int magic; // For debugging only. TODO: remove this in non-debug mode.
};

#define META_SIZE sizeof(struct block_meta)

void *sbrk(int increment);
struct block_meta *find_free_block(struct block_meta **last, size_t size);
struct block_meta *request_space(struct block_meta* last, size_t size);
void *malloc(size_t size);
struct block_meta *get_block_ptr(void *ptr);
void free(void *ptr);
void* realloc(void* ptr, size_t newsize);
void *kcalloc(uint32_t num, uint32_t size);
void kfree(void * ptr);

void *sbrk(int increment)
{
    static char global_mem[MEMORY_CAPACITY] = { 0 };
    static char *p_break = global_mem;

    char *const limit = global_mem + MEMORY_CAPACITY;
    char *const original = p_break;

    if (increment < global_mem - p_break  ||  increment >= limit - p_break)
    {
        errno = ENOMEM;
        return (void*)-1;
    }
    p_break += increment;

    return original;
}

void *global_base = NULL;

struct block_meta *find_free_block(struct block_meta **last, size_t size) {
    struct block_meta *current = global_base;
    while(current && !(current->free && current->size >= size)) {
        *last = current;
        current = current->next;
    }
    return current;
}

struct block_meta *request_space(struct block_meta* last, size_t size) {
    struct block_meta *block;
    block = sbrk(0);
    void *request = sbrk(size + META_SIZE);
    assert((void*)block == request); // Not thread safe.
    if(request == (void*) -1) {
        return NULL; // sbrk failed.
    }

    if(last) { // NULL on first request.
        last->next = block;
    }
    block->size = size;
    block->next = NULL;
    block->free = 0;
    block->magic = 0x12345678;
    return block;
}

void *malloc(size_t size) {
    struct block_meta *block;
    // TODO: align size?

    if (size <= 0) {
        return NULL;
    }

    if (!global_base) { // First call.
        block = request_space(NULL, size);
        if (!block) {
            return NULL;
        }
        global_base = block;
    } else {
        struct block_meta *last = global_base;
        block = find_free_block(&last, size);
        if (!block) { // Failed to find free block.
            block = request_space(last, size);
            if (!block) {
                return NULL;
            }
        } else {      // Found free block
            // TODO: consider splitting block here.
            block->free = 0;
            block->magic = 0x77777777;
        }
    }

    return(block + 1);
}

struct block_meta *get_block_ptr(void *ptr) {
    return (struct block_meta*)ptr - 1;
}

void free(void *ptr) {
    if (!ptr) {
        return;
    }

    // TODO: consider merging blocks once splitting blocks is implemented.
    struct block_meta* block_ptr = get_block_ptr(ptr);
    assert(block_ptr->free == 0);
    assert(block_ptr->magic == 0x77777777 || block_ptr->magic == 0x12345678);
    block_ptr->free = 1;
    block_ptr->magic = 0x55555555;
}

void* realloc(void* ptr, size_t newsize) {
    void* o = malloc(newsize);
    memcpy(o, ptr, sizeof(ptr));
    free(ptr);
    return o;
}

void *kcalloc(uint32_t num, uint32_t size) {
    void * ptr = malloc(num * size);
    memset(ptr, 0, num*size);
    return ptr;
}

void kfree(void * ptr) {
    free(ptr);
}

#endif