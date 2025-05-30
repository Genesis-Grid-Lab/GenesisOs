#include "idt.h"
#include "kernel/timer.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint64_t ticks;
const uint32_t freq = 100;

void on_Irq0(struct InterruptRegisters *regs) {
  ticks++;
  /* printf("timer tick: %u\n", ticks); */
}

void init_timer(uint32_t frequency){
  ticks = 0;
  irq_install_handler(0, &on_Irq0);

  uint32_t divisor = 1193180/frequency;

  // The divisor must be sent 8 bits at a time
  uint8_t low = (uint8_t) (divisor & 0xFF);
  uint8_t high = (uint8_t) ((divisor >> 8) & 0xFF);

  outb(0x43, 0x36);
  outb(0x40, low);
  outb(0x40, high);
}
