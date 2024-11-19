/**
 * @file vmcreate.c
 * @provides vmcreate
 *
 */
/* Embedded XINU, Copyright (C) 2023.  All rights reserved. */

#include <xinu.h>
#include <platform.h>

extern void *end;

/**
 * Creates the mappings for a user process.
 * @param pid the process id
 * @param stack the stack with any extra arguments and accounting information.  The page representing the stack is created in create.c.
 * @return the pagetable with all the mappings
 */
pgtbl vm_userinit(int pid, page stack)
{
    pgtbl pagetable = pgalloc();
    pcb *ppcb = &proctab[pid];

    // TODO: Once paging is working, you should be able to remove this line.  Then user processes will not be able to write to the serial driver.
	mapAddress(pagetable, UART_BASE, UART_BASE, PAGE_SIZE, PTE_R | PTE_W | PTE_U | PTE_A | PTE_D);

    // Map kernel code
    mapAddress(pagetable, (ulong)&_start, (ulong)&_start,
               ((ulong)&_ctxsws - (ulong)&_start), PTE_R | PTE_X | PTE_U | PTE_A | PTE_D | PTE_G);

    // Map interrupt and context switch
    mapAddress(pagetable, (ulong)&_ctxsws, (ulong)&_ctxsws, PAGE_SIZE + PAGE_SIZE, PTE_R | PTE_X | PTE_A | PTE_D | PTE_G);

    // Map rest of kernel code
    mapAddress(pagetable, (ulong)&_interrupte, (ulong)&_interrupte,
               ((ulong)&_datas - (ulong)&_interrupte), PTE_R | PTE_X | PTE_U | PTE_A | PTE_D | PTE_G);
               
    // Map global kernel structures and stack
    mapAddress(pagetable, (ulong)&_datas, (ulong)&_datas,
               ((ulong)memheap - (ulong)&_datas), PTE_R | PTE_U | PTE_A | PTE_D);

    // Map process stack
    mapPage(pagetable, stack, PROCSTACKADDR, PTE_R | PTE_W | PTE_U | PTE_A | PTE_D, SWAPAREAADDR);

    page swaparea = pgalloc();
    ppcb->swaparea = swaparea;
    ppcb->swaparea[CTX_KERNSP] = (ulong)_kernsp;
    mapPage(pagetable, swaparea, SWAPAREAADDR, PTE_R | PTE_W | PTE_A | PTE_D, SWAPAREAADDR);

    return pagetable;
}
