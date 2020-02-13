#include "common.h"
#define MAX_PAGE_PDE    8
unsigned int page_directory[1024] __attribute__((aligned(4096)));
unsigned int page_tables[MAX_PAGE_PDE][1024] __attribute__((aligned(4096)));


void init_page_directory() {
    int i;
    int j;
    for(i = 0; i < MAX_PAGE_PDE; i++)
    {
        // This sets the following flags to the pages:
        //   Supervisor: Only kernel-mode can access them
        //   Write Enabled: It can be both read from and written to
        //   Not Present: The page table is not present
        page_directory[i] = 0x00000002;
    }

    for (j=0; j<MAX_PAGE_PDE; j++) {
        for(i = 0; i < 1024; i++)
        {
            /*
             * We setup 1:1 memory mapping for the easy. 
             * VA = PA.
             */
            // (i * 0x1000):As the address is page aligned, it will always leave 12 bits zeroed.
            // (3):Those bits are used by the attributes ;)
            // (j*0x400000):As 4MiB mapping size for one of entrys of page_directory.
            //              For the first page_table, it need 0x0001000, 0x0002000....0x03ff000.
            //              For the second page_table, it needs start at 0x0400000, 0x0401000.
            page_tables[j][i] = ((i * 0x1000) + (j * 0x400000)) | 3; // attributes: supervisor level, read/write, present.
        }
        page_directory[j]=(unsigned int)page_tables[j] + (SYSSEG << 4) | 3;
    }


}

