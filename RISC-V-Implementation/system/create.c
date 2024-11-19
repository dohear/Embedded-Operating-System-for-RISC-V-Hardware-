/**
 * COSC 3250 - Project 4
 * this program creates 
 * @author Adam Samuelson & Daniel O'Hear
 * Instructor Dr. Brylow
 * TA-BOT:MAILTO adam.samuelson@marquette.edu daniel.ohear@marquette.edu
 */
/**
 * @file create.c
 * @provides create, newpid, userret
 *
 * COSC 3250 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */


#include <xinu.h>


static pid_typ newpid(void);
void userret(void);
void *pgalloc(void);


/**
 * Create a new process to start running a function.
 * @param funcaddr address of function that will begin in new process
 * @param ssize    stack size in bytes
 * @param name     name of the process, used for debugging
 * @param nargs    number of arguments that follow
 * @return the new process id
 */
syscall create(void *funcaddr, ulong ssize, unsigned int priority, char *name, ulong nargs, ...)
{
    ulong *saddr;               /* stack address                */
    ulong pid;                  /* stores new process id        */
    pcb *ppcb;                  /* pointer to proc control blk  */
    ulong i;
    va_list ap;                 /* points to list of var args   */
    ulong pads = 0;             /* padding entries in record.   */



    if (ssize < MINSTK)
        ssize = MINSTK;


    ssize = (ulong)((((ulong)(ssize + 3)) >> 2) << 2);
    /* round up to even boundary    */
    saddr = (ulong *)pgalloc();     /* allocate new stack and pid   */
    ulong *procStackAddr = saddr;
    pid = newpid();
    /* a little error checking      */
    if ((((ulong *)SYSERR) == saddr) || (SYSERR == pid))
    {
        return SYSERR;
    }


    numproc++;
    ppcb = &proctab[pid];
   
    // Setup PCB entry for new process.

    ppcb->pagetable = vm_userinit(pid, saddr);
    ppcb->tickets = priority; 
    ppcb->state = PRSUSP;                // Set process state to runnable
    ppcb->stkbase = saddr;         // Set stack base to base address of allocated stack
    ppcb->stklen = ssize;                 // Set stack length to the size of the allocated stack
    strncpy((*ppcb).name, name, PNMLEN);                    // Set process name
   
    //traverse to top of page table here

    saddr += 512;

    /* Initialize stack with accounting block. */
    *saddr = STACKMAGIC;
    *--saddr = pid;
    *--saddr = ppcb->stklen;
    *--saddr = (ulong)ppcb->stkbase;


    /* Handle variable number of arguments passed to starting function   */
    if (nargs)
    {
        pads = ((nargs - 1) / 8) * 8;
    }
    /* If more than 8 args, pad record size to multiple of native memory */
    /*  transfer size.  Reserve space for extra args                     */
    for (i = 0; i < pads; i++)
    {
        *--saddr = 0;
    }


    //populate the first 32 registers of the stack wtih 0s as fillers
    for(i = 0; i < 32; i++){
        *--saddr = 0;
    }




    // Initialize process context.
    *((saddr)+(CTX_PC)) = (ulong)funcaddr;      // Set program counter
    *((saddr)+(CTX_RA)) = (ulong)userret;     // set return address     
    *((saddr)+(CTX_SP)) = (ulong)saddr;  // Set stack pointer
 

    // Place arguments into activation record.
    // See K&R 7.3 for an example using va_start, va_arg, and va_end macros for variable argument functions.


    //stick with less than 8 args storing in context registers set empty to 0
    //where is activation record
    ulong ival;
    int w = 0;
    va_start(ap, 8);
    ulong *p;
    for(p = ap; *p; p++){
        ival = va_arg(ap, ulong);
        *(w + saddr) = ival;
        w++;

        if (pads != 0 && w == 8) w+=24;
    }
    ppcb->stkptr = saddr;

    saddr[CTX_SP] = (ulong)(procStackAddr + 512 - (ulong)ppcb->stkbase - (ulong)saddr);

    va_end(ap);
    return pid;
}

static pid_typ newpid(void)
{
    pid_typ pid;                /* process id to return     */
    static pid_typ nextpid = 0;

    for (pid = 0; pid < NPROC; pid++)
    {                           /* check all NPROC slots    */
        nextpid = (nextpid + 1) % NPROC;
        if (PRFREE == proctab[nextpid].state)
        {
            return nextpid;
        }
    }
    return SYSERR;
}

/**
 * Entered when a process exits by return.
 */
void userret(void)
{
    // ASSIGNMENT 5 TODO: Replace the call to kill(); with user_kill();
    // when you believe your trap handler is working in Assignment 5
    // user_kill();
    user_kill(); 
}
