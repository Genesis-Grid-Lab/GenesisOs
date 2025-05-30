#ifndef _PAGING_H_
#define _PAGING_H_

#include <stdint.h>

#define PAGE_SIZE 4096  // 4KB pages
#define PAGE_ENTRIES 1024 // 1024 entries per table

typedef struct {
    uint32_t entries[PAGE_ENTRIES];
} PageTable;

typedef struct {
    uint32_t entries[PAGE_ENTRIES];
} PageDirectory;

void init_paging(void);

#endif
