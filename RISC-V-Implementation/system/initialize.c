/**
 * @file initialize.c
 * The system begins intializing after the C environment has been
 * established.  After intialization, the null process remains always in
 * a ready or running state.
 */
/* Embedded Xinu, Copyright (C) 2009, 2020.  All rights reserved. */

#include <xinu.h>

/* Function prototypes */
static int sysinit(void);       /* intializes system structures          */
static void welcome(void);      /* Print inital O/S data                 */
process main(void);             /* main is the first process created     */

/* Declarations of major kernel variables */
pcb proctab[NPROC];             /* Process table                         */
qid_typ readylist;              /* List of READY processes               */

/* Active system status */
int numproc;                    /* Number of live user processes         */
int currpid;                    /* Id of currently running process       */

/* Params set by startup.S */
void *memheap;                  /* Bottom of heap (top of O/S stack)     */
ulong cpuid;                    /* Processor id                          */

ulong *_kernpgtbl;              /* Kernel page table address             */
ulong *_kernsp;                 /* Kernel stack pointer                  */
struct pgmemblk *pgfreelist = NULL;
                                /* Linked list of physical free pages    */
uint pgtbl_nents = 0;           /* Number of pages in the entire system  */

struct platform platform;       /* Platform specific configuration       */

interrupt_handler_t interruptVector[ALLWINNER_D1_NUM_IRQS];
/** Table of Xinu's interrupt handler functions.  
 * This is an array mapping IRQ numbers to handler functions.  
 */

process nullproc(void)
{
    while (1)
    {
        user_yield();
    }
}

void nulluser(void)
{
    /* Platform-specific initialization */
    platforminit();

    /* General initialization  */
    sysinit();

    /* Standard Embedded Xinu processor and memory info */
    welcome();

    /* Create the linked list of free physical pages    */
    // TODO: Uncomment these lines once you feel paging is working
    kprintf("Creating list of physical frames...\r\n");
    pgInit();

    /* Setup memory protection for kernel.  Turn paging on for the kernel.  */
    // TODO: Uncomment this line once you feel paging is working
    vm_kerninit();

    /* Call the main program */
    main();

    ready(create((void *)nullproc, INITSTK, PRIORITY_LOW, "prnull", 0),
          RESCHED_NO);
    kill(NULLPROC);
}

static void welcome(void)
{
    kprintf(VERSION);
    kprintf("\r\n\r\n");

    /* Output detected platform. */
    kprintf("Detected platform as: [%s] %s (ver %d) RV%dI %s \r\n\r\n",
            platform.manufacturer, platform.family, platform.revision,
            platform.architecture, platform.extensions);

    /* Output Xinu memory layout */
    kprintf("%10d bytes physical memory.\r\n",
            (ulong)platform.maxaddr - (ulong)platform.minaddr);
    kprintf("           [0x%016lX to 0x%016lX]\r\n",
            (ulong)platform.minaddr, (ulong)(platform.maxaddr - 1));

    kprintf("%10lu bytes reserved system area.\r\n",
            (ulong)_start - (ulong)platform.minaddr);
    kprintf("           [0x%016lX to 0x%016lX]\r\n",
            (ulong)platform.minaddr, (ulong)_start - 1);

    kprintf("%10d bytes Xinu code.\r\n", (ulong)&_end - (ulong)_start);
    kprintf("           [0x%016lX to 0x%016lX]\r\n",
            (ulong)_start, (ulong)&_end - 1);

    kprintf("%10d bytes kernel stack space.\r\n",
            (ulong)memheap - (ulong)&_end);
    kprintf("           [0x%016lX to 0x%016lX]\r\n",
            (ulong)&_end, (ulong)memheap - 1);
    kprintf("%10d bytes heap space.\r\n",
            (ulong)platform.maxaddr - (ulong)memheap);
    kprintf("           [0x%016lX to 0x%016lX]\r\n\r\n",
            (ulong)memheap, (ulong)platform.maxaddr - 1);

    kprintf("\r\n");
}

/**
 * Intializes all Xinu data structures and devices.
 * @return OK if everything is initialized successfully
 */
static int sysinit(void)
{

    int i = 0;
    pcb *ppcb = NULL;           /* process control block pointer */

    // Counter for unget buffer
    bufp = 0;

    /* Initialize system variables */
    /* Count this NULLPROC as the first process in the system. */
    numproc = 1;

    /* Initialize process table */
    for (i = 0; i < NPROC; i++)
    {
        proctab[i].state = PRFREE;
    }

    /* initialize null process entry */
    ppcb = &proctab[NULLPROC];
    ppcb->state = PRCURR;
    strncpy(ppcb->name, "main", 4);
    ppcb->stkbase = (void *)&_end;
    ppcb->stklen = (ulong)memheap - (ulong)&_end;
    ppcb->stkptr = NULL;
    /**
     * TODO: This won't compile properly until you add necessary changes to
     * proc.h
     */
    ppcb->tickets = PRIORITY_LOW;
    currpid = NULLPROC;

    readylist = newqueue();

    clkinit();

    seed_random(SEED);

    return OK;
}
