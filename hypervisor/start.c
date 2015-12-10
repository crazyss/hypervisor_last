#include "common.h"

#define SYSSEG  0x1000

struct wjn {
    int t;
    int w;
};

void init_gdtidt(void)
{
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
    struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) (ADR_IDT - (SYSSEG << 4));
    //struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) ADR_IDT;
    int i;
#if 0
    /* GDT<82>Ì<8f><89><8a>ú<89>» */
    for (i = 0; i <= LIMIT_GDT / 8; i++) {
        set_segmdesc(gdt + i, 0, 0, 0);
    }
    set_segmdesc(gdt + 2, 0xffffffff,   0x00009000, AR_CODE32_ER);
    set_segmdesc(gdt + 3, LIMIT_BOTPAK, ADR_BOTPAK, AR_DATA32_RW);
    load_gdtr(LIMIT_GDT, ADR_GDT);
#endif

    /* IDT<82>Ì<8f><89><8a>ú<89>» */
    for (i = 0; i <= LIMIT_IDT / 8; i++) {
        set_gatedesc(idt + i, 0xfff, 0, 0);
    }
    load_idtr(LIMIT_IDT, ADR_IDT);

    

    set_gatedesc(idt + 0x21, (int) inthandler21, 2 * 8, AR_INTGATE32);
#if 0
    /* IDT<82>Ì<90>Ý<92>è */
    set_gatedesc(idt + 0x27, (int) inthandler27, 2 * 8, AR_INTGATE32);
    set_gatedesc(idt + 0x2c, (int) inthandler2c, 2 * 8, AR_INTGATE32);
#endif

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


void kernelstart(char *arg)
{

    init_palette();
    char *vram = (char *)(0xa0000 - (SYSSEG << 4));
    unsigned short xsize,ysize;
    xsize=320;
    ysize=200;

    char *mcursor[256];
    int mx,my;

    init_gdtidt();
    init_pic();
    io_sti();

    static char font_A[16] = {
        0x00, 0x18, 0x18, 0x18, 0x18, 0x24, 0x24, 0x24,
        0x24, 0x7e, 0x42, 0x42, 0x42, 0xe7, 0x00, 0x00
    };


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


    //putfont8(vram, xsize,  8, 8, COL8_FFFFFF, font.Bitmap + ('A' - 31) * 16);
//    putfont8_string(vram,xsize, 8, 8, COL8_FFFFFF,font.Bitmap , "Hypervisor 2.1 Safety profile!!!");
    putfont8_string(vram,xsize, 8, 8, COL8_FFFFFF,font.Bitmap , "Hack Week 13!!!");
    //char buf[10];
    //sprintf(buf,"%d",vram);
    //putfont8_string(vram,xsize, 8, 8, COL8_FFFFFF,font.Bitmap , buf);

    /*draw a mouse on cetern screen */
    init_mouse_cursor8(mcursor, COL8_008484);
    mx = (xsize - 16) / 2;
    my = (ysize - 28 - 16) / 2;
    putblock8_8(vram, xsize, 16, 16, mx, my, mcursor, 16);


#if 0
    {
        int x,y;
        int i=0;
        const unsigned char (*pking)[16] = font.Bitmap;
        for (y=10;y<190;y+=30) {
            for (x=10;x<310;x+=10) {
                putfont8(vram, xsize, x, y, COL8_FFFFFF, pking+i);
                i++;
                if (i > 97)
                    goto out;
            }
        }
    }
out:

#endif


    //  putfont8_asc(vram, xsize, 10, 10, COL8_FFFFFF, 'c');


#if 0
    int i,j;
    int k=0;
    unsigned char *font = __font_bitmap__;
    for(i=10;i<10;i++){
        for (j=10;j<80;i+=8) {
            putfont8(vram, xsize, i, j, COL8_FFFFFF, font);
            font += ;
        }

    }
#endif


    io_out8(PIC1_DATA, 0xf9); /* PIC1<82>Æ<83>L<81>[<83>{<81>[<83>h<82>ð<8b><96><89>Â(11111001) */
    io_out8(PIC2_DATA, 0xef); /* <83>}<83>E<83>X<82>ð<8b><96><89>Â(11101111) */
    


    while(1)
        io_hlt();
    return ; 
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


void putfont8_string(char *vram, int xsize, int x, int y, char color,const unsigned char *font_bitmap, char * string)
{
    char ch ;
    for (y=10;y<190;y+=30) {
        for (x=10;x<310;x+=10) {
            ch = *string;
            if (ch == 0)
                return;
            putfont8(vram, xsize,  x, y, color, font_bitmap + (ch - 31) * 16);
            string++;
        }
    }
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

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
    char *str,*end;
    str = buf;
    end = buf + size;

    if (end < buf) {
        end = ((void*) -1);
        size = end - buf;
    }
    while(*fmt) {
        const char *old_fmt = fmt;

    }
}


void init_pic(void)
{
    io_out8(PIC1_DATA,  0xff); 
    io_out8(PIC2_DATA,  0xff); 

    io_out8(PIC1_COMMAND,   0x11);
    io_out8(PIC1_DATA, 0x20  );
    io_out8(PIC1_DATA, 1 << 2);
    io_out8(PIC1_DATA, 0x01);

    io_out8(PIC2_COMMAND,   0x11);
    io_out8(PIC2_DATA, 0x28  );
    io_out8(PIC2_DATA, 1 << 2);
    io_out8(PIC2_DATA, 0x01);

    io_out8(PIC1_DATA,  0xfb); 
    io_out8(PIC2_DATA,  0xff); 

    return;
}


void _inthandler21(int *esp)
{
    char *vram = (0xa0000 - (SYSSEG << 4));
    unsigned short xsize,ysize;
    xsize=320;
    ysize=200;

    //putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "INT 21 (IRQ-1) : PS/2 keyboard");
    putfont8_string(vram,320, 8, 40, COL8_FFFFFF,font.Bitmap , "INT 21 (IRQ-1) : PS/2 keyboard!!!");
    for (;;) {
        io_hlt();
    }
}

void _inthandler2c(int *esp)
{   
    char *vram = (char *)0xa0000;
    unsigned short xsize,ysize;
    xsize=320;
    ysize=200;


    //putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "INT 2C (IRQ-12) : PS/2 mouse");
    putfont8_string(vram,xsize, 8, 80, COL8_FFFFFF,font.Bitmap , "INT 2C (IRQ-12) : PS/2 mouse!!!");
    for (;;) {
        io_hlt();
    }
}

void _inthandler27(int *esp)
{
    io_out8(PIC1_COMMAND, 0x67); /* IRQ-07<8e>ó<95>t<8a>®<97>¹<82>ðPIC<82>É<92>Ê<92>m(7-1<8e>Q<8f>Æ) */
    return;
}

