#ifndef ARCH_I386_PAGING_H
#define ARCH_I386_PAGING_H

#include <stdint.h>

void paging_init();

struct page_directory_entry {
    uint32_t page_table_addr:20;
    char free:4;// Ignored, set to zero
    char S:1;   // Size: clear for 4iB, set for 1MiB pages
    char Z:1;   // Ignored, set to zero
    char A:1;   // Accessed, set by processor on page table page's access
    char PCD:1; // Caching yoke?
    char PWT:1; // Write through?
    char US:1;  // User/Supervisor: clear to disallow user-mode accesses, set to allow
    char RW:1;  // Read/Write: clear to disallow writes, set to allow
    char P:1;   // Present: cleared means no, set means yes
}__attribute__((packed));

struct page_table_entry {
    uint32_t page_addr:20;   // High 20 bits of physical address of 4 kiB page frame
    char free:3;
    char G:1;   // Global, clear to allow TLB update cached address if cr3 reset
    char PAT:1; // Some memory type yoke, clear for now
    char D:1;   // Dirty, set by processor on write to page
    char A:1;   // Accessed, set by processor on page access
    char PCD:1; // ?
    char PWT:1; // ?
    char US:1;  // User/Supervisor: clear to disallow user-mode accesses, set to allow
    char RW:1;  // Read/Write: clear to disallow writes, set to allow
    char P:1;   // Present: cleared means no, set means yes
}__attribute__((packed));

#endif
