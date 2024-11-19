#include <xinu.h>

/**
 * Puts a range of memory into the physical page list.
 * @param start the start address which will be rounded to the nearest page
 * @param end the ending address
 * @return OK if the entire range was able to be freed. SYSERR if an error occurs.
 */
syscall pgfreerange(void *start, void *end)
{
    if (end > platform.maxaddr || (char *)start < (char *)memheap
        || (char *)end < (char *)start)
    {
        return SYSERR;
    }

    char *pgstart = (char *)roundpage(start);
    for (; pgstart + PAGE_SIZE < (char *)end; pgstart += PAGE_SIZE)
    {
        if (SYSERR == pgfree(pgstart))
            return SYSERR;
    }

    return OK;
}

/**
 * Frees a single page
 * @param addr the address to put into the physical page list.  Must be at a page boundry
 * @return SYSERR if the address is not a page boundry. OK otherwise.
 */
syscall pgfree(void *addr)
{
    struct pgmemblk *page;

    if ((ulong)addr % PAGE_SIZE != 0)
        return SYSERR;

    page = (struct pgmemblk *)addr;
    page->next = pgfreelist;
    pgfreelist = page;

    return OK;
}
