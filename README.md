# Chuckles
````
|00| |00|  /00\  \0    0/ |000  x  |00\\  /00\  000|000 0|0 |000 |0\ 0  /00| |000  | O
|0000000| |0--0|  \0  0/  |0--  x  |00// |0--0|   0|0   0|0 |0-- |0\\0 |0    |0--  | |
|00| |00| |0  0|   \00/   |000  x  |00   |0  0|   0|0   0|0 |000 |0 \0  \00| |000  O |
````
## Design Considerations
### [Error Codes](https://www.gnu.org/fun/jokes/errno.2.html)
### [Haiku Messages](https://www.gnu.org/fun/jokes/error-haiku.html)
### Code Structuring
You should decide how your code should be structured. Consider that you will eventually port your operating system to different architectures, with different assembly instructions, with different initialisation sequences, with different hardware, with different memory structures, etc. You have to ensure you don't mix files from one architecture with files from another architecture. Meaty Skeleton serves as a minimal example of how to structure your code.
### Future Proofing
Consider that you will eventually want to add new features to the whole range of kernel interfaces. Therefore, you have to make sure it is easy to refactor them without breaking the dependant interfaces.
### Multithreading
Realise that, in some time from now, you will not run your code in a single thread, but it will be running in parallel with other threads and with other processors, meaning that it will be eventually preempted during a critical operation, and that some other thread will be able to corrupt the saved state.
### Hardware Abstraction
Realise that not every piece of hardware is present in every computer, so you might want to abstract it through a Hardware Abstraction Layer. For example PIT and HPET are two timers, which you might want to abstract into a single timing interface.
### Algorithms
For every task, find the algorithm that gets the most points from every aspect that matters to you (for example simplicity, speed, memory usage, etc).
### Paging
Paging is nice because it lets you map memory however you want and it lets processes see the full address space. It also provides advanced protection capabilities. You may wish to enable it early in boot.s.
### Higher Half
You should continue with a higher half kernel, so user-space programs can be loaded at 4 MiB (or lower if you like) without conflicting with the kernel binary. An important decision to take is where to exactly map the kernel.
Many prefer mapping the kernel to 0x80000000, leaving 2 GiB for kernel data and 2 GiB for processes. This can allow a monolithic kernel to cache large files or filesystem structures.
Others prefer mapping it to 0xC0000000, leaving 1 GiB for kernel data and 3 GiB for processes. Their main argument is that it integrates better with PAE, since the whole kernel space fits exactly in one page directory.
Some others (including the original author of this page) go extreme and map the kernel at 0xE0000000, leaving the tight space of 512 MiB for kernel data and 3.5 GiB for processes. Their main argument is that user-space should be able to use as much memory as possible.
### Permissions
Bare Bones taught you to use an ELF binary for the kernel. Do you know what do .text, .data, .rodata, .bss mean? Right, they are the sections of the executable. In .text the instructions for the processor are stored, in .data there is data, in .rodata there is read-only data, in .bss there is uninitialised data. There may be more sections, but let's focus on these for now.
Each of these sections should have different permissions applied, for example .text should be read-only, .data should be read-write, .rodata should be read-only, and .bss should be read-write. For these changes to take effect in kernel-space, you have to set the WP bit in the appropriate register. Make sure you disable execution of non-.text sections in x86-64 and PAE modes.
To facilitate setting permissions on sections you can do the following:
Tell the linker to align them at 4 KiB boundaries, so sections occupy whole pages.
Tell the linker to insert symbols indicating the start and end addresses of specific sections, so you can access them from your mapping code.
## More x86 Specific Things
An operating system should be self-reliant as much as possible. The bootloader may have left the environment in a "working" state, but it is not convenient in the long run.

### Segments
~~Create a GDT before the first segment change, because the one that GRUB has setup is no longer valid (the entries are simply cached in the processor still, that's why it "works").
You need at least these entries: null segment entry, kernel code segment entry, kernel data segment entry, user code segment entry, user data segment entry, task state segment entry.~~

### Interrupts
~~Every real operating system handles exceptions (for example page faults), and reads from peripherals only when data is received (instead of polling).
Create an IDT. Write interrupt handlers. Enable an interrupt controller (for example PIC or APIC).
Make sure to save all registers at the start of the interrupt handlers, and restore them at the end of the interrupt handlers. Keep also in mind that some exceptions cause an error code to be pushed to the stack, while some others do not.~~

### Timers
~~Initialise a timer to be able to keep track of timing. Consider which timer you would like to support first (most beginners go with PIT, although it is ancient), and how would you like to set it up (most set it up to tick at a convenient interval, like 1 ms or 10 ms). However, make sure you abstract the interface so adding support for more timers is easier.~~
## Memory Management
Soon you will need to allocate something whose size is not known at compile time. This is where the memory manager comes in.
### Getting the Memory Map
You first need to obtain the memory map, so you know which physical areas are free. Then you build on top of that.
### Physical Memory Manager
You for sure also need a list of free physical pages, so you know which physical page frames to allocate next.
A common way to do it is to create a linked list, that is to store the physical address of the next free page at the start of the previous free page, so only free memory is used to store it. However, you have paging enabled, so you can't arbitrarily write to every part of memory. You can instead map a page frame at a time, and write to it the address of the next free page.
### Virtual Memory Allocator
You will also need a way to allocate virtual pages for mapping physical memory, instead of hard-coded values. Get a way to track which parts of the address space are used, and which aren't.
There are multiple ways to track the address space. Modern operating systems like Linux and Windows use AVL trees, but you can also use whatever data structure you like.
### Heap Allocator
You will certainly want a heap as well, or do you want to keep allocating with 4KB granularity at a time? Start by implementing a very simple (but slow) linked list heap. You can then get to the more complicated designs, like separate block sizes for different buckets, etc. You should also keep in mind that eventually your heap will get out of memory, so you need to implement heap expansion.
Or you might opt for another design which involves a Slab Allocator.
## Scheduler
No operating system is real if it cannot schedule tasks. Every modern desktop operating system should allow browsing the web, while rendering a 3D scene, while sorting data in a spreadsheet, while writing a large file to the disk. This is taken care by the scheduler.
### Multiprocessing
Be prepared for multiprocessing. A scheduler that is not ready for multiprocessing will probably be fully rewritten.
### Priority Levels
Design the scheduler in a way, so threads can have different priority levels.
### Thread Lists
It is generally recommended there are different lists of threads for each state and priority level. This way, the scheduling code does not have to look through every thread for a high priority one, then possibly fail to find one, then iterate the thread list again to find a somewhat lower priority thread, then possibly fail again, etc. This means the scheduler code runs faster, since the lack of a thread of a specific priority level can be detected immediately, while also not iterating through inactive threads.
