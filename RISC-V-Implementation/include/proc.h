/**
 * @file proc.h
 * @provides isbadpid
 *
 * COSC 3250 / COEN 4830 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2010.  All rights reserved. */

#ifndef _PROC_H_
#define _PROC_H_

#include <hart.h>
#include <safemem.h>

/* process table declarations and defined constants                      */

#ifndef NPROC                 /*   if number of processes is not set,    */
#define NPROC       50        /**< set the number of processes           */
#endif

/* unusual value marks the top of the process stack                      */
#define STACKMAGIC 0x0A0AAA99

/* process state constants                                               */

#define PRFREE      0       /**< process slot is free                    */
#define PRCURR      1       /**< process is currently running            */
#define PRSUSP      2       /**< process is suspended                    */
#define PRREADY     3       /**< process is on ready queue               */

/* miscellaneous process definitions                                     */

#define PNMLEN      16      /**< length of process "name"                */

/* the null process is always eligible to run                            */
#define NULLPROC    0       /**< id of the null process                  */
#define BADPID     (-1)     /**< used when invalid pid needed            */

/* Reschedule constants for ready  */
#define RESCHED_YES 1           /**< tell ready to reschedule           */
#define RESCHED_NO  0           /**< tell ready not to reschedule       */

/** 
 * Check for invalid process ids.  Note that interrupts must be disabled
 * for the condition to hold true between statements.
 */
#define isbadpid(x) ((x)<0 || (x)>=NPROC || PRFREE == proctab[(x)].state)

/* process table entry */

typedef struct pentry
{
    int state;           /**< process state: PRCURR, etc.             */
    void *stkbase;       /**< base of run time stack                  */
    int stklen;          /**< stack length                            */
    void *stkptr;        /**< base of run time stack                  */
    char name[PNMLEN];   /**< process name                            */
    ulong tickets;       /**< priority in lottery scheduler           */
    pgtbl pagetable;     /**< process page table                      */
    ulong *swaparea;     /**< per-process swap area                   */
} pcb;

/* process initialization constants */
#define INITSTK  65536      /**< initial process stack size           */
#define INITRET  userret    /**< processes return address             */
#define MINSTK   4096       /**< minimum process stack size           */
#define NULLSTK  MINSTK     /**< null process stack size              */

/* Priority constants */
#define PRIORITY_LOW	1   /**< low process priority                 */
#define PRIORITY_MED	2   /**< medium process priority              */
#define PRIORITY_HIGH	3   /**< high process priority                */

extern struct pentry proctab[];
extern int numproc;         /**< currently active processes           */
extern int currpid;         /**< currently executing process          */

#endif                          /* _PROC_H_ */
