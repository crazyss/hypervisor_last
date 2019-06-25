#include "common.h"


int serial_received() {
   return io_in8(PORT + 5) & 1;
}

char read_serial() {
   char a;
    while (serial_received() == 0) {
        return '\0';
    }

   a=io_in8(PORT);

   return a;
}

int is_transmit_empty() {
   return io_in8(PORT + 5) & 0x20;
}
 
void write_serial(char a) {
    if (a == '\0') {
        return;
    }
    while (is_transmit_empty() == 0) {
        return;
    }
 
   io_out8(PORT,a);
}
