#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>

/**
 * outb:
 * Sends given data to given I/O port
 *
 * @param port I/O port to send to
 * @param data Data to send
 */
void outb(unsigned short port, unsigned char data);

uint8_t inb(uint16_t port);

inline void outb2(uint16_t Port, uint8_t Value){
  asm volatile ("outb %1, %0" : : "dN" (Port), "a" (Value));
}

inline char inb2(uint16_t port){
  char rv;
  asm volatile("inb %1, %0": "=a"(rv):"dN"(port));
  return rv;

}

struct InterruptRegisters{
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, csm, eflags, useresp, ss;
};

#endif
