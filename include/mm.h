#ifndef __MM_H
#define __MM_H

#define MEMMAN_FREES    4090    /*about 32KB */

struct FREEINFO {
    unsigned int addr, size;
};

struct MEMMAN {
    int frees, maxfrees, lostsize, losts;
    struct FREEINFO free[MEMMAN_FREES];
};

struct MEMMAP {
    unsigned int base;       //int has 32bit
    unsigned int res1;
    unsigned int length;
    unsigned int res2;
    unsigned int type;       // 1 can use
    unsigned int res3;
    unsigned int res4;
    unsigned int res5;
};

void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
#endif
