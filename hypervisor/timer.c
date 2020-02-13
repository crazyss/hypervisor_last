#include "common.h"
#include "timer.h"
#include "serial.h"

timer_zone timers;


void init_pit(void)
{
    /*
     * https://wiki.osdev.org/Programmable_Interval_Timer
     */

    int i;
    timer *t;
    /* bit - 7
     *
     * 0 0 = Channel 0
     *
     * 1 1 = Access mode: lobyte/hibyte
     *
     * 0 1 0 = Mode 2 (rate generator)
     * 
     * 0 BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
     * 
     * bit - 0
     */
    io_out8(PIT_CMD, 0x34);
    /*
     * Write two times, data port is 8bits.
     * so we have to write two times. This value helps PIT to generate 
     * a frequency.
     */
    io_out8(PIT_CH0, 0x9c);
    io_out8(PIT_CH0, 0x2e);


    /* timer_zone datastructure init */
    timers.count = 0;
    for (i=0;i<MAX_TIMER;i++) {
        timers.pool[i].flags = UNUSE;
    }
    /*pick up one of timers from pool*/
    t = timer_alloc();
    t->timeout = -1;
    t->next = 0;

    /* set current timer to new timer*/
    timers.current = t;
    timers.next = -1;

    return;
}
void timer_free(timer *t)
{
    t->flags = UNUSE; /* set to unused*/
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

    /*time is up, pick the current timer*/
    t = timers.current;
    while (t->timeout >= timers.count) {
        t->flags = USED;
        write_string_serial(".");
        t = t->next;
    }
    timers.current = t;
    timers.next = t->timeout;
    return;
}
