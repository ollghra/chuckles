#include <stdio.h>
#include <stdint.h>

#include <arch/i386/paging.h>

#include <sys/debug.h>

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t page_tables[1024][1024] __attribute__((aligned(4096)));

extern void load_new_page_directory(uint32_t page_directory_address);
//extern void load_page_directory(uint32_t page_directory_address);
//extern void enable_paging();

void paging_init()
{
    // Identity map everything
    for (uint32_t j = 0; j < 1024; j++)
    {
        for (uint32_t i = 0; i < 1024; i++)
        {
            page_tables[j][i] = 
                (((j<<22) + (i << 12)) & 0xFFFFF000)
                | (1 << 1)  // writes allowed
                | (1 << 0); // present
        }
        page_directory[j] = (((uint32_t)page_tables[j]) & 0xFFFFF000)
            | (1 << 1)
            | (1 << 0);
    }

    printf("Paging initialised with page directory at %x\n", (int)page_directory);
    printf("%d %dB PDEs, each with %d %dB PTEs, total %d kiB (%d MiB).\n"
            ,1024
            ,sizeof(page_directory[0])
            ,1024
            ,sizeof(page_tables[0][0])
            ,1024*1024*sizeof(page_tables[0][0])*sizeof(page_directory[0]) / (1<<10)
            ,1024*1024*sizeof(page_tables[0][0])*sizeof(page_directory[0]) / (1<<20)
          );
    printf("|PDE#    | PDE    | &PT    | PTE    | &PF    |\n"
           "|--------+--------+--------+--------+--------|\n");
    for (int i = 0; i < 4; i++)
    {
        printf("|%.8d|%.8X|%.8X|%.8X|%.8X|\n"
                ,i
                ,page_directory[i]
                ,page_directory[i] & 0xFFFFF000
                ,*(uint32_t*)(page_directory[i] & 0xFFFFF000)//page_tables[i][0]
                ,*(uint32_t*)(page_directory[i] & 0xFFFFF000) & 0xFFFFF000
                //,((struct page_table_entry*)(page_directory[i].page_table_addr<<12))->page_addr<<12
                );
    }
    printf("|PDE#    | PDE    | &PT    | PTE    | &PF    |\n"
           "|--------+--------+--------+--------+--------|\n");
    for (int i = 0; i < 4; i++)
    {
        printf("|%.8d|%.8X|%.8X|%.8X|%.8X|\n"
                ,0
                ,page_directory[0]
                ,page_directory[0] & 0xFFFFF000
                ,*(uint32_t*)((page_directory[0] & 0xFFFFF000) + i*sizeof(page_tables[0][0]))
                ,*(uint32_t*)((page_directory[0] & 0xFFFFF000) + i*sizeof(page_tables[0][0])) &0xFFFFF000 
                );
                //,((struct page_table_entry**)(page_directory[0].page_table_addr<<12))[i]->page_addr<<12);
    }
    // TODO: pass upper memory in from grub

    printf("Page directory at %p, loading cr3 with %X", page_directory, ((uint32_t)page_directory) & 0xFFFFF000);
    load_new_page_directory(((uint32_t)page_directory) & 0xFFFFF000);
    //enable_paging();


    //int i = 0;
    //for(; i < 1024; i++)
    //{
    //    page_directory[i] = 0x00000002;
    //}
    //uint32_t page_table[1024] __attribute__((aligned(4096)));
    //for (; i < 1024; i++)
    //{
    //    page_table[i] = (i * 0x1000) | 3; // 1 << 16
    //}
    //page_directory[0] = ((unsigned int ) &page_table) | 3;
    //serial_writed(page_directory);
    //serial_writes(" ");
    //serial_writed(page_directory[0]);
    //loadPageDirectory((unsigned int *)page_directory);
    //enablePaging();
}
