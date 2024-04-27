#include "string.h"

int strlen(const char* ptr)
{
    int len = 0;
    while (*ptr != 0)
    {
        len++;
        ptr += 1;
    }
    return len;
}

int strnlen(const char* ptr, int max)
{
    int len = 0;
    for (len = 0; len < max; len++) {
        if(ptr[len] == 0) {
            break;
        }
    }

    return len;
}

char* strcpy(char* dst, const char* src)
{
    char* ptr = dst;
    while (*src!= 0)
    {
        *ptr = *src;
        ptr += 1;
        src += 1;
    }
    *ptr = 0;
    return dst;
}

bool isdigit(char c)
{
    return c >= 48 && c <= 57;
}

int tonumericdigit(char c)
{
    return c - 48;
}