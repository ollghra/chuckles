#include <stdio.h>
#include <stdint.h>

#include <arch/i386/gdt.h>

struct gdt_entry
{
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access;
  uint8_t granularity;
  uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr
{
  unsigned short limit;
  unsigned int base;
} __attribute__((packed));

struct gdt_entry gdt[3];
struct gdt_ptr gp;

/* properly reload the new segment registers */
extern void gdt_flush();

void gdt_set_descriptor(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
  gdt[num].base_low = (base & 0xFFFF);
  gdt[num].base_middle = (base >> 16) & 0xFF;
  gdt[num].base_high = (base >> 24) & 0xFF;

  gdt[num].limit_low = (limit & 0xFFFF);
  gdt[num].granularity = ((limit >> 16) & 0x0F);

  gdt[num].granularity |= (gran & 0xF0);
  gdt[num].access = access;
}

void gdt_initialise()
{
  /* Setup the GDT pointer and limit */
  gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
  gp.base = (unsigned int) &gdt;

  /* NULL descriptor */
  gdt_set_descriptor(0, 0, 0, 0, 0);

  /* Code Segment
     Base: 0, Limit: 4GBytes, 4KB granularity, 32-bit opcodes*/
  gdt_set_descriptor(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

  /* Data Segment */
  gdt_set_descriptor(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

  /* Flush old GDT and install new changes */
  gdt_flush();
}

/*
  Possible future, cleaner implementation

  // Each define here is for a specific flag in the descriptor.
  // Refer to the intel documentation for a description of what each one does.
  #define SEG_DESCTYPE(x)  ((x) << 0x04) // Descriptor type (0 for system, 1 for code/data)
  #define SEG_PRES(x)      ((x) << 0x07) // Present
  #define SEG_SAVL(x)      ((x) << 0x0C) // Available for system use
  #define SEG_LONG(x)      ((x) << 0x0D) // Long mode
  #define SEG_SIZE(x)      ((x) << 0x0E) // Size (0 for 16-bit, 1 for 32)
  #define SEG_GRAN(x)      ((x) << 0x0F) // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
  #define SEG_PRIV(x)     (((x) &  0x03) << 0x05)   // Set privilege level (0 - 3)
 
  #define SEG_DATA_RD        0x00 // Read-Only
  #define SEG_DATA_RDA       0x01 // Read-Only, accessed
  #define SEG_DATA_RDWR      0x02 // Read/Write
  #define SEG_DATA_RDWRA     0x03 // Read/Write, accessed
  #define SEG_DATA_RDEXPD    0x04 // Read-Only, expand-down
  #define SEG_DATA_RDEXPDA   0x05 // Read-Only, expand-down, accessed
  #define SEG_DATA_RDWREXPD  0x06 // Read/Write, expand-down
  #define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
  #define SEG_CODE_EX        0x08 // Execute-Only
  #define SEG_CODE_EXA       0x09 // Execute-Only, accessed
  #define SEG_CODE_EXRD      0x0A // Execute/Read
  #define SEG_CODE_EXRDA     0x0B // Execute/Read, accessed
  #define SEG_CODE_EXC       0x0C // Execute-Only, conforming
  #define SEG_CODE_EXCA      0x0D // Execute-Only, conforming, accessed
  #define SEG_CODE_EXRDC     0x0E // Execute/Read, conforming
  #define SEG_CODE_EXRDCA    0x0F // Execute/Read, conforming, accessed
 
  #define GDT_CODE_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
  SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
  SEG_PRIV(0)     | SEG_CODE_EXRD
 
  #define GDT_DATA_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
  SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
  SEG_PRIV(0)     | SEG_DATA_RDWR
 
  #define GDT_CODE_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
  SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
  SEG_PRIV(3)     | SEG_CODE_EXRD
 
  #define GDT_DATA_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
  SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
  SEG_PRIV(3)     | SEG_DATA_RDWR
 
  void
  create_descriptor(uint32_t base, uint32_t limit, uint16_t flag)
  {
  uint64_t descriptor;
 
  // Create the high 32 bit segment
  descriptor  =  limit       & 0x000F0000;         // set limit bits 19:16
  descriptor |= (flag <<  8) & 0x00F0FF00;         // set type, p, dpl, s, g, d/b, l and avl fields
  descriptor |= (base >> 16) & 0x000000FF;         // set base bits 23:16
  descriptor |=  base        & 0xFF000000;         // set base bits 31:24
 
  // Shift by 32 to allow for low part of segment
  descriptor <<= 32;
 
  // Create the low 32 bit segment
  descriptor |= base  << 16;                       // set base bits 15:0
  descriptor |= limit  & 0x0000FFFF;               // set limit bits 15:0
 
  printf("0x%.16llX\n", descriptor);
  }
 
  int
  main(void)
  {
  create_descriptor(0, 0, 0);
  create_descriptor(0, 0x000FFFFF, (GDT_CODE_PL0));
  create_descriptor(0, 0x000FFFFF, (GDT_DATA_PL0));
  create_descriptor(0, 0x000FFFFF, (GDT_CODE_PL3));
  create_descriptor(0, 0x000FFFFF, (GDT_DATA_PL3));
 
  return 0;
  }
*/
