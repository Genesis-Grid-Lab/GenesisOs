#include <kernel/paging.h>

static PageDirectory page_directory __attribute__((aligned(4096)));
static PageTable page_tables[PAGE_ENTRIES] __attribute__((aligned(4096)));

void init_paging(void){

  for (int i = 0; i < PAGE_ENTRIES; i++) {
        page_directory.entries[i] = ((uint32_t)&page_tables[i]) | 3;  // Present & RW
        for (int j = 0; j < PAGE_ENTRIES; j++) {
            page_tables[i].entries[j] = (i * PAGE_ENTRIES + j) * PAGE_SIZE | 3;  // Identity map
        }
    }

    // Load page directory
    asm volatile ("mov %0, %%cr3" :: "r"(&page_directory));

    // Enable paging (set PG and PE bits in CR0)
    uint32_t cr0;
    asm volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile ("mov %0, %%cr0" :: "r"(cr0));
}
