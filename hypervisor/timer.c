#include "common.h"
#include "timer.h"
#include "serial.h"

timer_zone timers;


void init_pit(void)
{
    int i;
    io_out8(PIT_CMD, 0x34);
    io_out8(PIT_CH0, 0x9c);
    io_out8(PIT_CH0, 0x2e);

    timers.count = 0;
    for (i=0;i<MAX_TIMER;i++) {
        timers.pool[i].flags = 0;
    }
    timer *t;
    t = timer_alloc();
    t->timeout = -1;
    t->next = 0;
    timers.current = t;
    timers.next = -1;

    return;
}
void timer_free(struct TIMER *timer)
{
    timer->flags = UNUSE; /* set to unused*/
    return;
}


timer* timer_alloc(void)
{
    int i;
    for (i = 0; i < MAX_TIMER; i++) {
        if (timers.pool[i].flags == UNUSE) {
            timers.pool[i].flags = USED;
            return &timers.pool[i];
        }
    }
    return 0; /* no free */
}

void _inthandler20(int *esp)
{
    timer *t;
    io_out8(PIC0_COMMAND, PIC_EOI);
    timers.count++;
    if (timers.next > timers.count) {
        return;
    }
    t = timers.current;
    for(;;) {
        if (t->timeout > timers.count) {
            break;
        }
        t->flags = USED;
        write_string_serial(".");
        t = t->next;
    }
    timers.current = t;
    timers.next = t->timeout;
    return;
}
