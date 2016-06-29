#ifndef BOOT_CTYPE_H
#define BOOT_CTYPE_H
enum {
    false   = 0,
    true    = 1
};
static inline int isdigit(int ch)
{
    return (ch >= '0') && (ch <= '9');
}
static inline int isxdigit(int ch)
{
    if (isdigit(ch))
        return true;

    if ((ch >= 'a') && (ch <= 'f'))
        return true;

    return (ch >= 'A') && (ch <= 'F');
}

typedef unsigned int __size_t_32;
typedef unsigned long __size_t_64;
#ifndef SIZE_IN_64BIT
typedef __size_t_32 size_t;
#else
typedef __size_t_64 size_t;
#endif
#endif
