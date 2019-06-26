#ifndef _SERIAL_H
#define _SERIAL_H
#include "common.h"

int serial_received();
char read_serial();
int is_transmit_empty();
void write_serial(char a);
int write_string_serial(char *);
#endif
