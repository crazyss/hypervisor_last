#ifndef _MMU_H
#define _MMU_H

#define MAX_PAGE_PDE    8
extern void loadPageDirectory(unsigned int*);
extern void enablePaging();
extern unsigned int page_directory[MAX_PAGE_PDE] __attribute__((aligned(4096)));
extern unsigned int page_tables[MAX_PAGE_PDE][1024] __attribute__((aligned(4096)));
#endif
