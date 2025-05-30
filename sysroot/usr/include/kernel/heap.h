#ifndef _HEAP_H_
#define _HEAP_H_

#include <stdint.h>
#include <stddef.h>

#define HEAP_START 0x100000  // Heap starts at 1MB
#define HEAP_SIZE 0x100000   // 1MB heap size

typedef struct Block {
    size_t size;
    struct Block* next;
    int free;
} Block;

void init_heap(void);
void *malloc(size_t size);
void free(void* ptr);

#endif
