#ifndef _MMU_H
#define _MMU_H

#define MAX_PAGE_PDE    8
extern void loadPageDirectory(unsigned int*);
extern void enablePaging();
extern unsigned int page_directory[MAX_PAGE_PDE] __attribute__((aligned(4096)));
extern unsigned int page_tables[MAX_PAGE_PDE][1024] __attribute__((aligned(4096)));
#define MAX_FREE_MEM_BLOCK 10
#define PAGING_MEMORY (512 * 1024 * 1024)  //currently we map 512M , i suppose is enough        
#define PAGING_PAGES (PAGING_MEMORY >> 12)       
#define MAP_NR(addr) (((addr)) >> 12)  
#define USED 100                               
// each entry present one physical page(4K size), for example, mem_map[0]=100 means the physcial page(0-4K) is used.  
extern unsigned char mem_map [ PAGING_PAGES ];
// each entry store a free block memeory which detected by e820 bios int15 , for example free_memory[0][0] and free_memory[0][1] will store free memory start address and length.
extern int free_memory[MAX_FREE_MEM_BLOCK][2];
extern int max_free_mem;
#endif
