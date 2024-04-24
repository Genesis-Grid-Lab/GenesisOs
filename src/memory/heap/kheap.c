#include "kheap.h"
#include "heap.h"
#include "../../config.h"
#include "../../kernel.h"
#include "../memory.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

void kheap_init()
{
    int toatl_table_entries = GENOS_HEAP_SIZE_BYTES / GENOS_HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*)(GENOS_HEAP_TABLE_ADDRESS);
    kernel_heap_table.total = toatl_table_entries;

    void* end = (void*)(GENOS_HEAP_ADDRESS + GENOS_HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void*)(GENOS_HEAP_ADDRESS), end, &kernel_heap_table);
    if(res < 0)
    {
        print("Failed to create heap\n");
    }
}

void* kmalloc(size_t size)
{
    return heap_alloc(&kernel_heap, size);
}

void* kzalloc(size_t size)
{
    void* ptr = kmalloc(size);
    if(!ptr)
    {
        return 0;
    }
    memset(ptr, 0x00, size);
    return ptr;
}

void kfree(void* ptr)
{
    heap_free(&kernel_heap, ptr);
}