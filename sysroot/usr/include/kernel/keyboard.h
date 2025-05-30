#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "../arch/i386/io.h"
#include <stdint.h>

void init_Keyboard(void);
void keyboard_Handler(struct InterruptRegisters *regs);
const uint32_t get_key(void);

#endif
