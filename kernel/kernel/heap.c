#include "kernel/heap.h"

static uint8_t heap_memory[HEAP_SIZE];  // Heap memory
static Block *free_list = (Block *)heap_memory; // First free block

// Initialize heap
void init_heap(void) {
    free_list->size = HEAP_SIZE - sizeof(Block);
    free_list->next = NULL;
    free_list->free = 1;
}

// Find a free block
static Block* find_free_block(size_t size) {
    Block* current = free_list;
    while (current) {
        if (current->free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Split a block
static void split_block(Block* block, size_t size) {
    Block* new_block = (Block*)((uint8_t*)block + sizeof(Block) + size);
    new_block->size = block->size - size - sizeof(Block);
    new_block->free = 1;
    new_block->next = block->next;
    block->size = size;
    block->free = 0;
    block->next = new_block;
}

// Allocate memory
void* malloc(size_t size) {
    if (size == 0) return NULL;
    Block* block = find_free_block(size);
    if (!block) return NULL;
    
    if (block->size > size + sizeof(Block)) {
        split_block(block, size);
    }
    
    block->free = 0;
    return (void*)((uint8_t*)block + sizeof(Block));
}

// Free allocated memory
void free(void* ptr) {
    if (!ptr) return;
    Block* block = (Block*)((uint8_t*)ptr - sizeof(Block));
    block->free = 1;
}
