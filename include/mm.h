#ifndef __MM_H
#define __MM_H

#define MEMMAN_FREES    4090	/*about 32KB */

struct FREEINFO {
	unsigned int addr, size;
};

struct MEMMAN {
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};

struct MEMMAP {
	int base;		//int has 32bit
	int res1;
	int length;
	int res2;
	int type;		// 1 can use
	int res3;
	int res4;
	int res5;
};

void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
#endif
