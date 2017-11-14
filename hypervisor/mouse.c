#include "common.h"

const char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
};

void init_mouse_cursor8(char *mouse, char bc)
    /* <83>}<83>E<83>X<83>J<81>[<83>\<83><8b><82>ð<8f><80><94>õ<81>i16x16<81>j */
{
    int x, y;
    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            if (cursor[y][x] == '*') {
                mouse[y * 16 + x] = COL8_000000;
            }
            if (cursor[y][x] == 'O') {
                mouse[y * 16 + x] = COL8_FFFFFF;
            }
            if (cursor[y][x] == '.') {
                mouse[y * 16 + x] = bc;
            }
        }
    }
    return;
}


void putblock8_8(unsigned char *vram, int vxsize, int pxsize,
        int pysize, int px0, int py0, char *buf, int bxsize)
{
    int x, y;
    for (y = 0; y < pysize; y++) {
        for (x = 0; x < pxsize; x++) {
            vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
        }
    }
    return;
}

char mcursor[256];
#if 1
extern struct mouse_info mouse_status;
void draw_mouse_on_screen(struct mouse_info *p)
{
    int mx,my;
    int xsize,ysize;
    xsize=320;
    ysize=200;
		char buffer[20];
		mx = p->mx;
		my = p->my;
#if 0
		sprintf(buffer,"%p,%p", &mouse_status.mx,&mouse_status.my);
    putfont8_string(VRAM_ADDR,xsize, 8, 50, COL8_FFFFFF,font.Bitmap , buffer);
		sprintf(buffer,"%p,%p", &p->mx,&p->my);
    putfont8_string(VRAM_ADDR,xsize, 8, 60, COL8_FFFFFF,font.Bitmap , buffer);
#endif
    putblock8_8(VRAM_ADDR, xsize, 16, 16, mx, my, mcursor, 16);

}
#endif
