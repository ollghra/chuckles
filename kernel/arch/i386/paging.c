#include <stdio.h>
#include <stdint.h>

#include <arch/i386/paging.h>
/*
struct __attribute__((__packed__)) page_entry{

};

struct __attribute__((__packed__)) page_table{
  int P:1;
  int R:1;
  int U:1;
  int W:1;
  int D:1;
  int A:1;
  int ZERO:1;
  int S:1;
  int G:1;
  int free:3;
  long int addr:20;  
};

static inline void to_page_table(struct page_table *pt, uint32_t value)
{
  pt->P = value & 0x1;
  pt->R = (value & 0x2) >> 1;
  pt->U = (value & 0x4) >> 2;
  pt->W = (value & 0x8) >> 3;
  pt->D = (value & 0x10) >> 4;
  pt->A = (value & 0x20) >> 5;
  pt->ZERO = 0;
  pt->S = (value & 0x80) >> 7;
  pt->G = (value & 0x100) >> 8;
  pt->free = (value & ( 0x7 << 9 )) >> 9;
  pt->addr = (value & 0xFFFFF000) >> 12;
}

static inline uint32_t from_page_table(struct page_table *pt)
{
  printf("%d %d %d %d %d %d %d %d %d %d %d ", pt->P, pt->R, pt->U, pt->W, pt->D, pt->A, pt->ZERO, pt->S, pt->G, pt->free, pt->addr);
  return pt->P +
    ((uint32_t)pt->R << 1) +
    ((uint32_t)pt->U << 2) +
    ((uint32_t)pt->W << 3) +
    ((uint32_t)pt->D << 4) +
    ((uint32_t)pt->A << 5) +
    ((uint32_t)pt->ZERO << 6) +
    ((uint32_t)pt->S << 7) +
    ((uint32_t)pt->G << 8) +
    ((uint32_t)pt->free << 9) +
    ((uint32_t)pt->addr << 12);
}
*/
extern void loadPageDirectory(uint32_t*);
extern void enablePaging();
extern void test_page_fault();
int init_paging()
{
  //struct page_table page_directory[1024];
  uint32_t page_directory[1024] __attribute__((aligned(4096)));
  int i;
  for( i = 0; i < 1024; i++)
    {
      //to_page_table(&page_directory[i], 0x00000002);
      page_directory[i] = 0x00000002;
    }
  
  uint32_t first_page_table[1024] __attribute__((aligned(4096)));
  /* holds the physical address where we want to start mapping 
     these pages to. in this case, we want to map these pages 
     to the very beginning of memory.*/
 
  //we will fill all 1024 entries in the table, mapping 4 megabytes
  for(i = 0; i < 1024; i++)
    {
      // As the address is page aligned, it will always leave 12 bits zeroed.
      // Those bits are used by the attributes ;)
      first_page_table[i] = (i * 0x1000) | 3;
      // attributes: supervisor level, read/write, present.
    }
  // attributes: supervisor level, read/write, present
  page_directory[0] = ((uint32_t)first_page_table) | 3;

  loadPageDirectory(page_directory);
  enablePaging();
  test_page_fault();
  return 0;
}
