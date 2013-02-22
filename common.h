#include "font.h"
#include <stdio.h>
#include <stdarg.h>

void io_hlt(void); 
void io_cli(void); 
void io_out8(int port, int data); 
int io_load_eflags(void); 
void io_store_eflags(int eflags); 
 
void init_palette(void); 
void set_palette(int start, int end, unsigned char *rgb); 
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1); 
 
void putfont8(char *vram, int xsize, int x, int y, char c, const unsigned char *font);
void putfont8_string(char *vram, int xsize, int x, int y, char color,const unsigned char *font_bitmap, char * string);

extern struct bitmap_font font;

#define COL8_000000     0
#define COL8_FF0000     1
#define COL8_00FF00     2
#define COL8_FFFF00     3
#define COL8_0000FF     4
#define COL8_FF00FF     5
#define COL8_00FFFF     6
#define COL8_FFFFFF     7
#define COL8_C6C6C6     8
#define COL8_840000     9
#define COL8_008400     10
#define COL8_848400     11
#define COL8_000084     12
#define COL8_840084     13
#define COL8_008484     14
#define COL8_848484     15



struct SEGMENT_DESCRIPTOR {
    short limit_low, base_low;
    char base_mid, access_right;
    char limit_high, base_high;
};

struct GATE_DESCRIPTOR {
    short offset_low, selector;
    char dw_count, access_right;
    short offset_high;
};

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);

/*8259 interrupt controller*/

#define PIC1    0x20
#define PIC2    0xa0
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1+1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2+2)



/*GDT / IDT data*/

#define ADR_IDT         0x0026f800
#define LIMIT_IDT       0x000007ff
#define ADR_GDT         0x00270000
#define LIMIT_GDT       0x0000ffff
#define ADR_BOTPAK      0x00280000
#define LIMIT_BOTPAK    0x0007ffff
#define AR_DATA32_RW    0x4092
#define AR_CODE32_ER    0x409a
#define AR_INTGATE32    0x008e

extern void inthandler21(void);
extern void inthandler27(void);
extern void inthandler2c(void);
