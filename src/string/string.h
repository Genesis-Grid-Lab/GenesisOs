#ifndef STRING_H
#define STRING_H
#include <stdbool.h>

char tolower(char s1);
int strlen(const char *str);
int strnlen(const char* ptr, int max);
int strnlen_terminator(const char* str, int max, char terminator);
int istrncmp(const char* ptr1, const char* ptr2, int max);
int strncmp(const char* ptr1, const char* ptr2, int max);
char* strcpy(char* dst, const char* src);
bool isdigit(char c);
int tonumericdigit(char c);

#endif // STRING_H