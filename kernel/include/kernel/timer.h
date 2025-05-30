#ifndef _TIMER_H_
#define _TIME_H_

#include "../arch/i386/io.h"

void init_timer(uint32_t frequency);
void on_Irq0(struct InterruptRegisters* regs);

#endif
