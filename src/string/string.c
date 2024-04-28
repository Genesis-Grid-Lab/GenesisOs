#include "string.h"

char tolower(char s1)
{
    if (s1 >= 'A' && s1 <= 'Z')
        return s1 + 32;
    else
        return s1;
}

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

int strnlen_terminator(const char* str, int max, char terminator)
{
    int i = 0;
    for (i = 0; i < max; i++) {
        if(str[i] == terminator || str[i] == '\0') {
            break;
        }
    }
    return i;
}

int istrncmp(const char* ptr1, const char* ptr2, int max)
{
    unsigned char c1, c2;

    while(max-- > 0) {
        c1 = (unsigned char)*ptr1++;
        c2 = (unsigned char)*ptr2++;
        if(c1!= c2 && tolower(c1)!= tolower(c2)) {
            return c1 - c2;
        }
        if(c1 == '\0') {
            return 0;
        }
    }

    return 0;
}
int strncmp(const char* ptr1, const char* ptr2, int max)
{
    unsigned char c1, c2;

    while(max-- > 0) {
        c1 = (unsigned char)*ptr1++;
        c2 = (unsigned char)*ptr2++;
        if(c1!= c2) {
            return c1 - c2;
        }
        if(c1 == '\0') {
            return 0;
        }
    }
    return 0;
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