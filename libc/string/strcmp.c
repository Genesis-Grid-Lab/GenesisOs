#include <string.h>
#include "../../kernel/include/kernel/heap.h"

int strcmp(char* first, char* second) {
  while (*first != 0 && *second != 0) {
    if (*first > *second) return 1;
    if (*first < *second) return -1;
    ++first;
    ++second;
  }
  if (*first == *second) return 0;
  return *first > *second ? 1 : -1;
}

char *strcpy(char *s1, const char *s2)
{
    char *s1_p = s1;
    while (*s1++ = *s2++)
      ;
    return s1_p;
}

int strncmp(const char *s1, const char *s2, size_t n){
  for (size_t i = 0; i < n; i++) {
        int diff = (unsigned char)s1[i] - (unsigned char)s2[i];
        if (diff != 0 || s1[i] == '\0')
            return diff;
    }
    return 0;
}

char *strchr (register const char *s, int c){
  do {
    if (*s == c)
      {
	return (char*)s;
      }
  } while (*s++);
  return (0);
}

void strncpy( char* _dst, const char* _src, size_t _n ){
  size_t i = 0;
   while(i++ != _n && (*_dst++ = *_src++));
}

static char *olds;

char *strtok (char *s, const char *delim){
  static int currIndex = 0;
    if(!s || !delim || s[currIndex] == '\0')
    return NULL;
    char *W = (char *)malloc(sizeof(char)*100);
    int i = currIndex, k = 0, j = 0;

    while (s[i] != '\0'){
        j = 0;
        while (delim[j] != '\0'){
            if (s[i] != delim[j])
                W[k] = s[i];
            else goto It;
            j++;
        }

        i++;
        k++;
    }
It:
    W[i] = 0;
    currIndex = i+1;
    //Iterator = ++ptr;
    return W;
}
