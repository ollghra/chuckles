#include <stdio.h>
#include <stdint.h>

#include <arch/i386/paging.h>

#include <sys/debug.h>
/*
struct page_directory_entry {
  int page_table_addr:20;
  char free:3;
  char G:1;
  char S:1;
  char Z:1; // Always set to 0
  char A:1;
  char D:1;
  char W:1;
  char U:1;
  char R:1;
  char P:1;
}__attribute__((packed));

struct page_directory_entry page_directory[1024];

struct page_table_entry {
  int page_addr:20;
  char free:3;
  char G:1;
  char Z:1; // Always set to 0
  char D:1;
  char A:1;
  char C:1;
  char W:1;
  char U:1;
  char R:1;
  char P:1;
}__attribute__((packed));

extern void loadPageDirectory(unsigned int*);
extern void enablePaging();

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

  struct page_table_entry page_table[1024];

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
uint32_t page_directory[1024] __attribute__((aligned(4096)));

extern void loadPageDirectory(unsigned int*);
extern void enablePaging();

void init_paging()
{
  int i = 0;
  for(; i < 1024; i++)
    {
      page_directory[i] = 0x00000002;
    }
  uint32_t page_table[1024] __attribute__((aligned(4096)));
  for (; i < 1024; i++)
    {
      page_table[i] = (i * 0x1000) | 3; // 1 << 16
    }
  page_directory[0] = ((unsigned int ) &page_table) | 3;
  serial_writed(page_directory);
  serial_writes(" ");
  serial_writed(page_directory[0]);
  loadPageDirectory((unsigned int *)page_directory);
  enablePaging();

}
