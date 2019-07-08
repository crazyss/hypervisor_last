#include "common.h"

void init_pit(void)
{
    io_out8(PIT_CMD, 0x34);
	io_out8(PIT_CH0, 0x9c);
	io_out8(PIT_CH0, 0x2e);
	return;
}

void _inthandler20(int *esp)
{
	unsigned char data;
	io_out8(PIC0_COMMAND, PIC_EOI);
    write_string_serial(".");
	return;
}
