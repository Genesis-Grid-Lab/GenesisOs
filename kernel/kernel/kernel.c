#include <stdint.h>
#include <stdio.h>
#include <kernel/kernel.h>
#include <kernel/tty.h>
#include <kernel/timer.h>
#include <kernel/keyboard.h>
#include <kernel/memory.h>
#include <kernel/heap.h>
#include <kernel/paging.h>
#include <kernel/fs.h>
#include "gdt.h"
#include "idt.h"

void hello_main(int argc, char* argv[]) {
    printf("Hello, OS!\n");
    if (argc > 1) {
        printf("Argument: %s\n", argv[1]);
    }
}

void kernel_main(uint32_t magic, struct multiboot_info* bootInfo) {
  terminal_init();
  init_GDT();
  printf("GDT done ....\n");
  init_IDT();
  printf("IDT done ...\n");
  init_timer(100);
  printf("Timer done ...\n");
  init_Keyboard();
  printf("Keyboard done ...\n");
  init_memory(bootInfo);
  printf("Memory done ...\n");
  init_heap();
  printf("Heap done ...\n");
  init_paging();
  printf("Paging done ...\n");
  fs_create("bin", DIR_TYPE);
  fs_create("bin/hello", FILE_TYPE);
  fs_register_executable("bin/hello", hello_main);
  show_logo();
  start_little_shell();
}

void show_logo(void){
  terminal_clear();
  printf("\n\n\n\n\n\n\n\n");
  terminal_set_color(10); // 10 = lightgreen
  printf("                --------|   |--------   |-------\n");
  printf("                |   __  |   |  ____  |  |  _____|\n");
  printf("                |  | |__|   |  |   | |  |  |\n");
  printf("                |  |        |  |   | |  |  -----\n");
  printf("                |  ||---|   |  |   | |  |_____  |\n");
  printf("                |  ||_  |   |  |___| |   _____| |\n");
  printf("                |_______|   |_______ |  |_______|\n");
  terminal_set_color(15); // white
}
