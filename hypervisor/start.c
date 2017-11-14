#include "common.h"
#include "fifo8.h"


void enable_mouse(void);
void init_keyboard(void);
void keyboard_handler(unsigned char data);
void mouse_handler(unsigned char data);
extern char mcursor[256];

struct wjn {
    int t;
    int w;
};

struct fifo key_fifo;
struct fifo mouse_fifo;
unsigned char keybuffer[32];
unsigned char mousebuffer[128];
unsigned char james_global;

struct screen_postion {
    int x;
    int y;
};
struct screen_postion screen_pos = {
    .x = 0,
    .y = 20,
};

struct mouse_info mouse_status = {0};

void init_gdtidt(void)
{
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) (ADR_GDT - (SYSSEG << 4));
    struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) (ADR_IDT - (SYSSEG << 4));
    //struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) ADR_IDT;
    int i;
    /* GDT<82>Ì<8f><89><8a>ú<89>» */
    for (i = 0; i <= LIMIT_GDT / 8; i++) {
        set_segmdesc(gdt + i, 0, 0, 0);
    }
#if 0
    set_segmdesc(gdt + 2, 0xffffffff,   0x00010000, AR_CODE32_ER);
    set_segmdesc(gdt + 3, LIMIT_BOTPAK, ADR_BOTPAK, AR_DATA32_RW);
    load_gdtr(LIMIT_GDT, ADR_GDT);
#endif
    /* IDT<82>Ì<8f><89><8a>ú<89>» */
    for (i = 0; i <= LIMIT_IDT / 8; i++) {
        set_gatedesc(idt + i, 0xfff, 0, 0);
    }
    load_idtr(LIMIT_IDT, ADR_IDT);

    

    set_gatedesc(idt + 0x21, (int) inthandler21, 2 * 8, AR_INTGATE32);
#if 1
    /* IDT<82>Ì<90>Ý<92>è */
    set_gatedesc(idt + 0x27, (int) inthandler27, 2 * 8, AR_INTGATE32);
#endif

    set_gatedesc(idt + 0x2c, (int) inthandler2c, 2 * 8, AR_INTGATE32);
    return;
}


void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
    if (limit > 0xfffff) {
        ar |= 0x8000; /* G_bit = 1 */
        limit /= 0x1000;
    }
    sd->limit_low    = limit & 0xffff;
    sd->base_low     = base & 0xffff;
    sd->base_mid     = (base >> 16) & 0xff;
    sd->access_right = ar & 0xff;
    sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
    sd->base_high    = (base >> 24) & 0xff;
    return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
    gd->offset_low   = offset & 0xffff;
    gd->selector     = selector;
    gd->dw_count     = (ar >> 8) & 0xff;
    gd->access_right = ar & 0xff;
    gd->offset_high  = (offset >> 16) & 0xffff;
    return;
}
void drawing_desktop()
{
    unsigned char *vram = VRAM_ADDR;
    unsigned short xsize,ysize;
    xsize=320;
    ysize=200;
    char buf[50];
    unsigned int memtotal;
    struct MEMMAN *memman = (struct MEMMAN *) (MEMMAN_ADDR - (SYSSEG << 4));
    memman_init(memman);
    int ret;

    static char font_A[16] = {
        0x00, 0x18, 0x18, 0x18, 0x18, 0x24, 0x24, 0x24,
        0x24, 0x7e, 0x42, 0x42, 0x42, 0xe7, 0x00, 0x00
    };
		/*init mouse global data*/
    mouse_status.mx = (xsize - 16) / 2;
    mouse_status.my = (ysize - 28 - 16) / 2;
    init_mouse_cursor8(mcursor, COL8_008484);

    boxfill8(vram, xsize, COL8_008484,  0,         0,          xsize -  1, ysize - 29);
    boxfill8(vram, xsize, COL8_C6C6C6,  0,         ysize - 28, xsize -  1, ysize - 28);
    boxfill8(vram, xsize, COL8_FFFFFF,  0,         ysize - 27, xsize -  1, ysize - 27);
    boxfill8(vram, xsize, COL8_C6C6C6,  0,         ysize - 26, xsize -  1, ysize -  1);


    boxfill8(vram, xsize, COL8_FFFFFF,  3,         ysize - 24, 59,         ysize - 24);
    boxfill8(vram, xsize, COL8_FFFFFF,  2,         ysize - 24,  2,         ysize -  4);
    boxfill8(vram, xsize, COL8_848484,  3,         ysize -  4, 59,         ysize -  4);
    boxfill8(vram, xsize, COL8_848484, 59,         ysize - 23, 59,         ysize -  5);
    boxfill8(vram, xsize, COL8_000000,  2,         ysize -  3, 59,         ysize -  3);
    boxfill8(vram, xsize, COL8_000000, 60,         ysize - 24, 60,         ysize -  3);

    boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 24, xsize -  4, ysize - 24);
    boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 23, xsize - 47, ysize -  4);
    boxfill8(vram, xsize, COL8_FFFFFF, xsize - 47, ysize -  3, xsize -  4, ysize -  3);
    boxfill8(vram, xsize, COL8_FFFFFF, xsize -  3, ysize - 24, xsize -  3, ysize -  3);

    memtotal=memtest(0x400000, 0xbfffffff);
    ret = memman_free(memman, 0x00001000, 0x0009e000);
    if (ret != 0) {
        putfont8_string(vram,xsize, 28, 48, COL8_FFFFFF,font.Bitmap , "Memman Free 1 Failed");
    }
    ret = memman_free(memman, 0x00400000, memtotal - 0x00400000);
    if (ret != 0) {
        putfont8_string(vram,xsize, 28, 48, COL8_FFFFFF,font.Bitmap , "Memman Free 2 Failed");
    }
    sprintf(buf,"MEMORY %d MB. %dMB Heap Free.", memtotal / (1024*1024), memman_total(memman) / (1024*1024));
    putfont8_string(vram,xsize, 8, 8, COL8_FFFFFF,font.Bitmap , "Hack Week 0x10!!!");
    putfont8_string(vram,xsize, 8, 28, COL8_FFFFFF,font.Bitmap , buf);

		draw_mouse_on_screen(&mouse_status);
}

void kernelstart(char *arg)
{

    init_palette();

    init_gdtidt();
    init_pic();
		fifo_init(&key_fifo, 32, keybuffer);
		fifo_init(&mouse_fifo, 128, mousebuffer);
    io_sti();



    io_out8(PIC0_DATA, 0xf9); /* PIC0<82>Æ<83>L<81>[<83>{<81>[<83>h<82>ð<8b><96><89>Â(11111001) */
    io_out8(PIC1_DATA, 0xef); /* <83>}<83>E<83>X<82>ð<8b><96><89>Â(11101111) */

		init_keyboard();

		enable_mouse();



    drawing_desktop();

		//draw_mouse_on_screen();
    while(1) {
			io_cli();
			if (fifo_status(&key_fifo) <= 0 && fifo_status(&mouse_fifo) <= 0)	{
				io_stihlt();
			}else {
					if (fifo_status(&key_fifo) > 0) {
						unsigned char data = fifo_get(&key_fifo);
						io_sti();
						keyboard_handler(data);
					}else if (fifo_status(&mouse_fifo) > 0) {
						unsigned char data = fifo_get(&mouse_fifo);
						io_sti();
						mouse_handler(data);
					}
			}
		}

    return ; 
}


#define PORT_KEYDAT				0x0060
#define PORT_KEYSTA				0x0064
#define PORT_KEYCMD				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

void wait_KBC_sendready(void)
{
	/* 等待键盘控制电路准备完毕 */
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

void init_keyboard(void)
{
	/* 初始化键盘控制电路 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

void enable_mouse(void)
{
	/* 激活鼠标 */
//	unsigned char status;
//	wait_KBC_sendready();
//	io_out8(PORT_KEYCMD, 0x20);
//	wait_KBC_sendready();
//	status = ((char)io_in8(PORT_KEYDAT)|0x2);
//	status &= 0xDF;
//	wait_KBC_sendready();
//	io_out8(PORT_KEYCMD, 0x60);
//	wait_KBC_sendready();
//	io_out8(PORT_KEYDAT, status);
//
//
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	return; /* 顺利的话，键盘控制器会返回ACK(0xfa) */
}

#if 0
void putfont8(char *vram, int xsize, int x, int y, char c, const unsigned char *font_bitmap)
{
    int i,j;
    char *p, d /* data */;
    for (i = 0; i < 16; i++) {
        p = vram + (y + i) * xsize + x;
        for(j=0;j<8;j++) {
            if(font_bitmap[i*8+j]=='X') {
                p[j]=c;
            }
        }
    }
    return;
}
#endif

void putfont8(char *vram, int xsize, int x, int y, char c, const unsigned char *font_bitmap)
{
    int i;
    char *p, d /* data */;
    for (i = 0; i < 16; i++) {
        p = vram + (y + i) * xsize + x;
        d = font_bitmap[i];
        if ((d & 0x80) != 0) { p[0] = c; }
        if ((d & 0x40) != 0) { p[1] = c; }
        if ((d & 0x20) != 0) { p[2] = c; }
        if ((d & 0x10) != 0) { p[3] = c; }
        if ((d & 0x08) != 0) { p[4] = c; }
        if ((d & 0x04) != 0) { p[5] = c; }
        if ((d & 0x02) != 0) { p[6] = c; }
        if ((d & 0x01) != 0) { p[7] = c; }
    }
    return;
}


void putfont8_string(unsigned char *vram, int xsize, int x, int y, char color,const unsigned char *font_bitmap, unsigned char * string)
{
    char ch ;
    int x1,y1;
    if (x == 0)
        x1 = screen_pos.x;
    else
        x1 = x;
    if (y == 0)
        y1 = screen_pos.y;
    else
        y1 = y;

    for (;y1<190;y1+=30) {
        for (;x1<310;x1+=10) {
            ch = *string;
            if (ch == 0)
                break;
            putfont8(vram, xsize,  x1, y1, color, font_bitmap + (ch - 31) * 16);
            string++;

        }
        if (x == 0 && y == 0) {
            screen_pos.x += 2;
            if (screen_pos.x >= 300) {
                screen_pos.y += 12;
                screen_pos.x = 0;
            }
        }
    }
    //screen_pos.y += 1;

}


void set_palette(int start, int end, unsigned char *rgb)
{
    int i, eflags;
    io_out8(0x03c8, start);
    for (i = start; i <= end; i++) {
        io_out8(0x03c9, rgb[0] / 4);
        io_out8(0x03c9, rgb[1] / 4);
        io_out8(0x03c9, rgb[2] / 4);
        rgb += 3;
    }
    return;
}

void init_palette(void)
{
    static unsigned char table_rgb[16 * 3] = {
        0x00, 0x00, 0x00,   /*  0:*/
        0xff, 0x00, 0x00,   /*  1:*/
        0x00, 0xff, 0x00,   /*  2:*/
        0xff, 0xff, 0x00,   /*  3:*/
        0x00, 0x00, 0xff,   /*  4:*/
        0xff, 0x00, 0xff,   /*  5:*/
        0x00, 0xff, 0xff,   /*  6:*/
        0xff, 0xff, 0xff,   /*  7:*/
        0xc6, 0xc6, 0xc6,   /*  8:*/
        0x84, 0x00, 0x00,   /*  9:*/
        0x00, 0x84, 0x00,   /* 10:*/
        0x84, 0x84, 0x00,   /* 11:*/
        0x00, 0x00, 0x84,   /* 12:*/
        0x84, 0x00, 0x84,   /* 13:*/
        0x00, 0x84, 0x84,   /* 14:*/
        0x84, 0x84, 0x84    /* 15:*/
    };
    set_palette(0, 15, table_rgb);
    return;

    /* static char <96>$<97>ß<82>Í<81>A<83>f<81>[<83>^<82>É<82>µ<82>©<8e>g<82>¦<82>È<82>¢<82>¯<82>ÇDB<96>$<97>ß<91><8a><93><96> */
}


void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
    int x, y;
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++)
            vram[y * xsize + x] = c;
    }
    return;
}


void init_pic(void)
{

    io_out8(PIC1_OCW1,  0xFF); 
    io_out8(PIC0_OCW1,  0xFF); 

    io_out8(PIC0_COMMAND,   0x11);
    io_out8(PIC1_COMMAND,   0x11);


    io_out8(PIC1_DATA, 0x28  );
    io_out8(PIC0_DATA, 0x20  );

    io_out8(PIC0_DATA, 1 << 2);
    io_out8(PIC1_DATA, 2);

    io_out8(PIC0_DATA, 0x01); /*8086 mode for both*/
    io_out8(PIC1_DATA, 0x01);

    io_out8(PIC0_DATA,  0xfb); 
    io_out8(PIC1_DATA,  0xff); 

    return;
}


#define PORT_KEYDAT 0x0060
void keyboard_handler(unsigned char data)
{
    unsigned char *vram = VRAM_ADDR;
    unsigned short xsize,ysize;
    unsigned char out_buffer[3];
    unsigned char t;
    xsize=320;
    ysize=200;
    if (data >= 0x81 || data == -1)
        return ;
    out_buffer[0] = scancode[data];
    if (out_buffer[0] == 0) {
#if 1
        t=(data & 0xf0)>>4;
        if (t >= 0xa && t <= 0xf) {
            out_buffer[0] = 'A' + t - 10; 
        }else {
            out_buffer[0] = '0' + t;
        }
        t=(data & 0xf);
        if (t >= 0xA && t <= 0xF) {
            out_buffer[1] = 'A' + t - 10; 
        }else if (t >= 0 && t <= 9){
            out_buffer[1] = '0' + t;
        }
        out_buffer[2]=0;

        boxfill8(vram, xsize, COL8_C6C6C6,  0,         ysize - 28, xsize -  1, ysize - 28);
        boxfill8(vram, xsize, COL8_FFFFFF,  0,         ysize - 27, xsize -  1, ysize - 27);
        boxfill8(vram, xsize, COL8_C6C6C6,  0,         ysize - 26, xsize -  1, ysize -  1);

        boxfill8(vram, xsize, COL8_FFFFFF,  3,         ysize - 24, 59,         ysize - 24);
        boxfill8(vram, xsize, COL8_FFFFFF,  2,         ysize - 24,  2,         ysize -  4);
        boxfill8(vram, xsize, COL8_848484,  3,         ysize -  4, 59,         ysize -  4);
        boxfill8(vram, xsize, COL8_848484, 59,         ysize - 23, 59,         ysize -  5);
        boxfill8(vram, xsize, COL8_000000,  2,         ysize -  3, 59,         ysize -  3);
        boxfill8(vram, xsize, COL8_000000, 60,         ysize - 24, 60,         ysize -  3);

        boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 24, xsize -  4, ysize - 24);
        boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 23, xsize - 47, ysize -  4);
        boxfill8(vram, xsize, COL8_FFFFFF, xsize - 47, ysize -  3, xsize -  4, ysize -  3);
        boxfill8(vram, xsize, COL8_FFFFFF, xsize -  3, ysize - 24, xsize -  3, ysize -  3);

        putfont8_string(vram,xsize, 283, 180, COL8_00FF00,font.Bitmap , out_buffer);
#endif
        return;
    }
    out_buffer[1] = 0;

        
    putfont8_string(vram,xsize, 0, 0, COL8_FFFFFF,font.Bitmap , out_buffer);

    if (data == 0x30 || data == 0xB0) {
        putfont8_string(vram,xsize, 8, 100, COL8_FFFFFF,font.Bitmap , (unsigned char *)"May the source be with you!");
    }
}

void _inthandler21(int *esp)
{
    unsigned char data;
    io_out8(PIC0_COMMAND, 0x61);
    data = io_in8(PORT_KEYDAT);
		fifo_put(&key_fifo, data);
		return;
}

void mouse_handler(unsigned char data)
{
    unsigned char *vram = VRAM_ADDR;
    unsigned short xsize,ysize;
		char buffer[20];
    xsize=320;
    ysize=200;
		if (mouse_status.phase == 0) {
			if (data == 0xfa) {
					mouse_status.phase = 1;
			}
			return ;
		}else if (mouse_status.phase == 1) {
			if ((data & 0xc8) == 0x08) {
				mouse_status.buf[0] = data;
				mouse_status.phase = 2;
			}
			return;
		}else if (mouse_status.phase == 2) {
			mouse_status.buf[1] = data;
			mouse_status.phase = 3;
			return ;
		}else if (mouse_status.phase == 3) {
			mouse_status.buf[2] = data;
			mouse_status.phase = 1;
			mouse_status.btn = mouse_status.buf[0] & 0x07;
			mouse_status.x = mouse_status.buf[1];
			mouse_status.y = mouse_status.buf[2];
			
			if ((mouse_status.buf[0] & 0x10) != 0) {
				mouse_status.x |= 0xffffff00;
			}
			if ((mouse_status.buf[0] & 0x20) != 0) {
				mouse_status.y |= 0xffffff00;
			}
			mouse_status.y = -mouse_status.y;
			boxfill8(VRAM_ADDR, xsize, COL8_008484, mouse_status.mx, mouse_status.my, mouse_status.mx + 15, mouse_status.my + 15);
			//mouse_status.mx += mouse_status.x;
			mouse_status.mx += mouse_status.x;
			mouse_status.my += mouse_status.y;
			if (mouse_status.mx < 0) {
				mouse_status.mx = 0;
			}
			if (mouse_status.my < 0) {
				mouse_status.my = 0;
			}
			if (mouse_status.mx > xsize - 16) {
				mouse_status.mx = xsize - 16;
			}
			if (mouse_status.my > ysize - 16) {
				mouse_status.my = ysize - 16;
			}
			draw_mouse_on_screen(&mouse_status);
			return;
		}
		return;
}
void _inthandler2c(int *esp)
{   
		unsigned char data;
		io_out8(PIC1_OCW2, 0x64);
		io_out8(PIC0_OCW2, 0x62);
		data = io_in8(PORT_KEYDAT);
		fifo_put(&mouse_fifo, data);
		return;
}

void _inthandler27(int *esp)
{
    io_out8(PIC0_COMMAND, 0x67); /* IRQ-07<8e>ó<95>t<8a>®<97>¹<82>ðPIC<82>É<92>Ê<92>m(7-1<8e>Q<8f>Æ) */
    return;
}

