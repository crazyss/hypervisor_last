#ifndef _TIMER_H
#define _TIMER_H
#include "common.h"

#define MAX_TIMER   128

#define UNUSE   0
#define USED    1

typedef struct timer{
    struct timer *next;
    unsigned int timeout, flags;
    int data;
}timer;

typedef struct timer_zone{
    unsigned int count, next; /*next means timeout*/
    timer *current;
    timer pool[MAX_TIMER];
}timer_zone;
void init_pit(void);
timer* timer_alloc(void);
#endif
