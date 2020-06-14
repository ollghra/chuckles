#include <stdio.h>
#include <stdint.h>

#include <arch/i386/paging.h>

#include <sys/debug.h>

/*
void init_paging()
{
    int i = 0;
    for (; i < 1024; i++)
    {
        page_directory[i].page_table_addr = 0;
        page_directory[i].free = 0;
        page_directory[i].G = 0;
        page_directory[i].S = 0;
        page_directory[i].Z = 0; // always set to 0
        page_directory[i].A = 0;
        page_directory[i].D = 0;
        page_directory[i].W = 0;
        page_directory[i].U = 0; // Supervisor only
        page_directory[i].R = 1; // set r/w bit
        page_directory[i].P = 0; // not present
    }

    unsigned int phys_addr = 0; // first page physical address

    for(i = 0; i < 1024; i++)
    {
        page_table[i].page_addr = phys_addr + 1024 * i;
        page_table[i].free = 0;
        page_table[i].G = 0;
        page_table[i].Z = 0; // Always set to 0
        page_table[i].D = 0;
        page_table[i].A = 0;
        page_table[i].C = 0;
        page_table[i].W = 0;
        page_table[i].U = 0;
        page_table[i].R = 1;
        page_table[i].P = 1;
    }

    page_directory[0].page_table_addr = (int) &page_table;
    page_directory[0].R = 1;
    page_directory[0].P = 1;

    loadPageDirectory((unsigned int *) &page_directory);
    serial_writed((unsigned int)page_directory[0]);
    enablePaging();
}
*/

struct page_directory_entry page_directory[1024] __attribute__((aligned(4096)));
struct page_table_entry page_tables[1024][1024] __attribute__((aligned(4096)));

extern void load_new_page_directory(uint32_t page_directory_address);
//extern void load_page_directory(uint32_t page_directory_address);
//extern void enable_paging();

void paging_init()
{
    // Identity map everything
    for (int i = 0; i < 1024; i++)
    {
        page_directory[i].page_table_addr = 0;
        page_directory[i].free  = 0;
        page_directory[i].S     = 0;
        page_directory[i].Z     = 0;
        page_directory[i].A     = 0;
        page_directory[i].PCD   = 0;
        page_directory[i].PWT   = 0;
        page_directory[i].US    = 0;
        page_directory[i].RW    = 1;
        page_directory[i].P     = 0;
    }

    for (uint32_t j = 0; j < 1024; j++)
    {
        for (uint32_t i = 0; i < 1024; i++)
        {
            page_tables[j][i].page_addr = (((j<<22) + (i << 12)) & 0xFFFFF000) >> 12;
            page_tables[j][i].free = 0;
            page_tables[j][i].G    = 0;
            page_tables[j][i].PAT  = 0;
            page_tables[j][i].D    = 0;
            page_tables[j][i].A    = 0;
            page_tables[j][i].PCD  = 0;
            page_tables[j][i].PWT  = 0;
            page_tables[j][i].US   = 0;
            page_tables[j][i].RW   = 1;
            page_tables[j][i].P    = 1;
        }
        page_directory[j].page_table_addr = (((uint32_t)page_tables[j]) & 0xFFFFF000) >> 12;
        page_directory[j].P = 1;
    }

    printf("Paging initialised with page directory at %x\n", (int)page_directory);
    printf("%d %dB PDEs, each with %d %dB PTEs, total %d kiB (%d MiB).\n"
            ,1024
            ,sizeof(struct page_directory_entry)
            ,1024
            ,sizeof(struct page_table_entry)
            ,1024*1024*sizeof(struct page_table_entry)*sizeof(struct page_directory_entry) / (1<<10)
            ,1024*1024*sizeof(struct page_table_entry)*sizeof(struct page_directory_entry) / (1<<20)
          );
    printf("|PDE#    | PDE    | &PT    | PTE    | &PF    |\n"
           "|--------+--------+--------+--------+--------|\n");
    for (int i = 0; i < 4; i++)
    {
        printf("|%.8d|%.8X|%.8X|%.8X|%.8X|\n"
                ,i
                ,(page_directory[i])
                ,page_directory[i].page_table_addr<<12
                ,(page_tables[i][0])
                ,((struct page_table_entry*)(page_directory[i].page_table_addr<<12))->page_addr<<12
                );
    }
    printf("|PDE#    | PDE    | &PT    | PTE    | &PF    |\n"
           "|--------+--------+--------+--------+--------|\n");
    for (int i = 0; i < 4; i++)
    {
        printf("|%.8d|%.8X|%.8X|%.8X|%.8X|\n"
                ,0
                ,*page_directory
                ,((struct page_table_entry*)((page_directory[0].page_table_addr<<12)+i*sizeof(struct page_table_entry)))
                ,*((struct page_table_entry*)((page_directory[0].page_table_addr<<12)+i*sizeof(struct page_table_entry)))
                //,(page_tables[0][i])
                ,page_tables[0][i].page_addr << 12
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
