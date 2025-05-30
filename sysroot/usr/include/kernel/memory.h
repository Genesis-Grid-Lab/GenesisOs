#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdint.h>
#include <stdio.h>
#include "kernel/multiboot.h"

void init_memory(struct multiboot_info* bootInfo);

#endif
