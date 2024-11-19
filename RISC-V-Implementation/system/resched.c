/**
 * @file resched.c
 * @provides resched
 *
 * COSC 3250 Assignment 6
 */
/* Embedded XINU, Copyright (C) 2008,2024.  All rights reserved. */

#include <xinu.h>

extern void ctxsw(void *, void *, ulong);
int get_total_tickets(void);
/**
 * Reschedule processor to next ready process.
 * Upon entry, currpid gives current process id.  Proctab[currpid].pstate 
 * gives correct NEXT state for current process if other than PRREADY.
 * @return OK when the process is context switched back
 */
syscall resched(void)
{
    pcb *oldproc;               /* pointer to old process entry */
    pcb *newproc;               /* pointer to new process entry */
    pcb *ppcb = &proctab[currpid];

    oldproc = &proctab[currpid];

    /* place current process at end of ready queue */
    if (PRCURR == oldproc->state)
    {
        oldproc->state = PRREADY;
        enqueue(currpid, readylist);
    }

    /**
     * We recommend you use a helper function for the following:
     * TODO: Get the total number of tickets from all processes that are in
     * current and ready states.
     * Use random() function to pick a random ticket. 
     * Traverse through the process table to identify which proccess has the
     * random ticket value.  Remove process from queue.
     * Set currpid to the new process.
     */



    int total_tickets = get_total_tickets();
    int random_ticket = random(total_tickets);    
    int selected_pid;
    int i = 0;
    int ticket_counter;
    pcb *checkedProc = &proctab[i];

    for(i=0; i < NPROC; i++){
        if (checkedProc->state == PRCURR || checkedProc->state == PRREADY){
            ticket_counter += checkedProc->tickets;
        }
        if (random_ticket < ticket_counter)
        {
            selected_pid = i;
            break;
        }
        checkedProc = &proctab[i];
    }

    newproc = &proctab[currpid];
    newproc->state = PRCURR;    /* mark it currently running    */

#if PREEMPT
    preempt = QUANTUM;
#endif

    ctxsw(&oldproc->stkptr, &newproc->stkptr, (MAKE_SATP(currpid, newproc->pagetable)));

    /* The OLD process returns here when resumed. */
    return OK;
}

int get_total_tickets(void){

    int total_tickets = 0;
    int i = 0;
    pcb *checkedProc = &proctab[i];


    for (i=0; i < NPROC; i++){
        if (checkedProc->state == PRCURR || checkedProc->state == PRREADY){
            total_tickets += proctab[i].tickets;
        }
        checkedProc = &proctab[i];
    }
    return total_tickets;
}