#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char* __restrict, ...);
/* int *printf_number(int* argp, int length, bool sign, int radix); */
int put_char(int);
int puts(const char*);

#ifdef __cplusplus
}
#endif

#endif
