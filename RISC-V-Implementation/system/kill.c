/**
 * @file kill.c
 * Provides: kill
 *
 * COSC 3250/ COEN 4820 Assignment 4
 */

/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <xinu.h>

/*
 * kill  --  kill a process and remove it from the system
 */
syscall kill(int pid)
{
    pcb *ppcb;                  /* points to process control block for pid */

    if (isbadpid(pid) || (PRFREE == (ppcb = &proctab[pid])->state))
    {
        return SYSERR;
    }

    ppcb = &proctab[pid];

    numproc = numproc - 1;

    switch (ppcb->state)
    {
    case PRCURR:
        ppcb->state = PRFREE;   /* suicide */
        resched();
        // The process should never run this line after resched is called.
        break;
    case PRREADY:
        remove(pid);
        ppcb->state = PRFREE;
        break;
    default:
        ppcb->state = PRFREE;
        break;
    }

    return OK;
}