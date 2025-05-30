#include "kernel/memory.h"
#include "kernel/multiboot.h"

void init_memory(struct multiboot_info* bootInfo){
  for(int i = 0; i < 7; i += sizeof(struct multiboot_mmap_entry)){
    struct multiboot_mmap_entry *mmmt = (struct multiboot_mmap_entry*)(bootInfo->mmap_addr + i);

    printf("Loq addr: %x | High Addr: %x | low Length: %x | High Length: %x | size: %x | Type: %u\n", mmmt->addr_low, mmmt->addr_high, mmmt->len_low,
	   mmmt->len_high, mmmt->size, mmmt->type);
  }
}
