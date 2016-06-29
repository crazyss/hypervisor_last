#include "ctype.h"
size_t strnlen(const char *s, size_t maxlen)
{
    const char *es = s;
    while (*es && maxlen) {
        es++;
        maxlen--;
    }           

    return (es - s); 
}               

