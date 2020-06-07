/* The bootloader will look at this image and start execution at the symbol
   designated at the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Kernel lives at 3GiB+1MiB in virtual address space
     * Mapped to 1MiB in physical address space */
	. = 0xC0100000;

	kernel_start_marker = .;
	/* First put the multiboot header, as it is required to be put very early in the image
     * or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text ALIGN(4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.multiboot)
		*(.text)
	}

	/* Read-only data. */
	.rodata ALIGN(4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	/* Read-write data (initialised) */
	.data ALIGN(4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}

	/* Read-write data (uninitialised) and stack */
	.bss ALIGN(4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
        *(.bootstrap_stack)
	}

	/* The compiler may produce other sections,
	   put them in the proper place in this file
	   if you'd like to include them in the final kernel. */

	.endkernel ALIGN(4K) : AT(ADDR(.endkernel) - 0xC0000000)
	{
		kernel_end_marker = .;
		LONG(0xDEADBABA);
	}
}
