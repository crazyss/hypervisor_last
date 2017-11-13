#include "common.h"

const static char cursor[16][16] = {
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

#if 1
void draw_mouse_on_screen()
{
    int mx,my;
    int xsize,ysize;
    char mcursor[256];
    xsize=320;
    ysize=200;

    init_mouse_cursor8(mcursor, COL8_008484);
    mx = (xsize - 16) / 2;
    my = (ysize - 28 - 16) / 2;
    putblock8_8(VRAM_ADDR, xsize, 16, 16, mx, my, mcursor, 16);
}
#endif
