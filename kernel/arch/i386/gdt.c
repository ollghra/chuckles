#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <arch/i386/gdt.h>
#include <arch/i386/tss.h>

#include <sys/debug.h>

//struct gdt_entry gdt[5];

#define NUM_GDT_ENTRIES 6
uint64_t gdt[NUM_GDT_ENTRIES];

struct gdt_ptr gp;

void create_descriptor(int num, uint32_t base, uint32_t limit, uint16_t flag)
{
	uint64_t descriptor;

	// Create the high 32 bit segment
	descriptor  =  limit       & 0x000F0000; // set limit bits 19:16
	descriptor |= (flag <<  8) & 0x00F0FF00; // set type, p, dpl, s, g, d/b, l and avl fields
	descriptor |= (base >> 16) & 0x000000FF; // set base bits 23:16
	descriptor |=  base        & 0xFF000000; // set base bits 31:24

	// Shift by 32 to allow for low part of segment
	descriptor <<= 32;

	// Create the low 32 bit segment
	descriptor |= base  << 16;				// set base bits 15:0
	descriptor |= limit  & 0x0000FFFF;		// set limit bits 15:0

	gdt[num] = descriptor;
}

// properly reload the new segment registers and load gdt
extern void gdt_flush(uintptr_t gdtp);
// load tss
extern void tss_flush();

tss_entry_t tss;
static void write_tss(uint32_t num, uint16_t ss0, uint32_t esp0)
{
	create_descriptor(num, (uint32_t) &tss, ((uint32_t) &tss) + sizeof(tss), (GDT_TSS));
	memset(&tss, 0, sizeof(tss));
	tss.ss0 = ss0;
	tss.esp0 = esp0;
    tss.cs = 0x0b;
    tss.ss = 0x13;
    tss.ds = 0x13;
    tss.es = 0x13;
    tss.fs = 0x13;
    tss.gs = 0x13;
	tss.iomap_base = sizeof(tss);	
}

void gdt_initialise()
{
	gp.limit = (sizeof(struct gdt_entry) * NUM_GDT_ENTRIES) - 1;
	gp.base = (uint32_t) &gdt;
	serial_writes("&GDT: ");
	serial_writes(itoa(gp.base));
	serial_writec('\n');


	create_descriptor(0, 0, 0, 0);
	create_descriptor(1, 0, 0xFFFFFFFF, (GDT_CODE_PL0));
	create_descriptor(2, 0, 0xFFFFFFFF, (GDT_DATA_PL0));
	create_descriptor(3, 0, 0xFFFFFFFF, (GDT_CODE_PL3));
	create_descriptor(4, 0, 0xFFFFFFFF, (GDT_DATA_PL3));
	
	/* Set up TSS */
	write_tss(5,0x10,0);

	gdt_flush((uintptr_t)&gp);
	serial_writes("GDT flushed\n");
	tss_flush();
	serial_writes("TSS flushed\n");
}

