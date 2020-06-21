#include <stdint.h>
#include <stdio.h>

void * physical(void * virtual)
{
    uint32_t pdidx = (uint32_t) virtual >> 22;
    uint32_t ptidx = (uint32_t) virtual >> 12 & 0x3FF;
    uint32_t * pd = (uint32_t *) 0xFFFFF000; // Stored as last PDE
    if (!(*(pd + pdidx)& 0x01))
    {
        printf("PDE %d not present.\n",pdidx);
        return (void *)0;
    }
    uint32_t * pt = ((uint32_t*) 0xFFC00000) + (0x400 * pdidx);
    if (!(*pt & 0x01))
    {
        printf("Page table not present.\n");
        return (void *)0;
    }
    return (void *)((pt[ptidx] & ~0xFFF) + ((uint32_t)virtual & 0xFFF));
}

void * virtual_kaddr(void * physical)
{
    return (void *) (0xC0000000 + (uint32_t) physical);
}
