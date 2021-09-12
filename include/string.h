
#ifndef _STRING_H
#define _STRING_H
#include "ctype.h"

#define NULL 0ULL
size_t strnlen(const char *s, size_t maxlen);
int strncmp(const char *s1, const char *s2, size_t n);
#endif
