#ifndef _TIMER_H
#define _TIMER_H
#include "common.h"

#define MAX_TIMER   128

#define UNUSE   0
#define USED    1

typedef struct TIMER {
    struct TIMER *next;
    unsigned int timeout, flags;
    int data;
}timer;

typedef struct TIMER_POOL {
    unsigned int count, next;
    struct TIMER *current;
    struct TIMER pool[MAX_TIMER];
}timer_zone;
void init_pit(void);
timer* timer_alloc(void);
#endif
