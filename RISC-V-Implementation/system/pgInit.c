#include <xinu.h>

/**
 * Initialize the physical pages by calling pgfreerange across the entire
 * avaliable memory space.  This should be done before any paging is setup.
 */
void pgInit(void)
{
    /* number of pages in memory */
    pgtbl_nents =
        roundpage((ulong)platform.maxaddr - (ulong)memheap) / PAGE_SIZE;

    pgfreerange(memheap, platform.maxaddr);
}

