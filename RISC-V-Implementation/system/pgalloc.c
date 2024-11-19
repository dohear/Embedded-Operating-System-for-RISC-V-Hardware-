/**
 * @file pgalloc
 * Gets a single free physical page
 *
 */
/* Embedded Xinu, Copyright (C) 2023.  All rights reserved. */

#include <xinu.h>

/**
 * Gets a single free physical page.
 * @return the address of the page or if there are no more pages avaliable, a SYSERR.
 */
void *pgalloc(void)
{
    struct pgmemblk *page;

    if (pgfreelist == NULL)
    {
        return (void *)SYSERR;
    }

    page = pgfreelist;
    pgfreelist = page->next;

    // Clears the data in the page
    bzero((char *)page, PAGE_SIZE);

    return (void *)page;
}