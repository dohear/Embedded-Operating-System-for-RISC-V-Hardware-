/**
 * @file dispatch.c
 * @provides create, newpid, userret
 *
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */


#include <xinu.h>
#include <interrupt.h>

/**
 * 
 * @ingroup process
 * Dispatch the trap or exception handler, called via interrupt.S
 * @param cause  The value of the scause register 
 * @param stval  The value of the stval register  
 * @param frame  The stack pointer of the process that caused the interupt 
 * @param program_counter  The value of the sepc register 
 */

ulong dispatch(ulong cause, ulong val, ulong *frame, ulong *program_counter) {
    ulong swi_opcode;
    
    pcb *ppcb = &proctab[currpid];

    if((long)cause > 0) {
        cause = cause << 1;
        cause = cause >> 1;

        /**
        * Check to ensure the trap is an environment call from U-Mode (e call in interrupt.h)
        * Find the system call number that's triggered
        * Pass the system call number and any arguments into syscall_dispatch. Make sure to set the return value in the appropriate spot.
        * Update the program counter appropriately with set_sepc
        *
        * If the trap is not an environment call from U-Mode call xtrap
        */
       
        // Check if the trap is an environment call from U-Mode
        if (cause == E_ENVCALL_FROM_UMODE) {
            //Find the system call number that's triggered
            swi_opcode = *(ulong *)(*program_counter);
            ulong syscall_number = ppcb->swaparea[CTX_A7]; //Extracting the syscall number

            //Pass the system call number and any arguments into syscall_dispatch
            ulong syscall_retval = syscall_dispatch(syscall_number, &ppcb->swaparea[CTX_A0]);

            //Set the return value in the appropriate spot
            ppcb->swaparea[CTX_A0] = syscall_retval;

            //Update the program counter appropriately with set_setpc and move to next instruction
            set_sepc((ulong)(program_counter) + 4);
        } 
        else {
            // If the trap is not an environment call from U-Mode call xtrap
            xtrap(ppcb->swaparea, cause, val, program_counter);
        }
    }
    else {
        cause = cause << 1;
        cause = cause >> 1;
        uint irq_num;

        volatile uint *int_sclaim = (volatile uint *)(PLIC_BASE + 0x201004);
        irq_num = *int_sclaim;

        if(cause == I_SUPERVISOR_EXTERNAL) {
            interrupt_handler_t handler = interruptVector[irq_num];

            *int_sclaim = irq_num;
            if (handler)
            {
                (*handler) ();
            } else {
                kprintf("ERROR: No handler registered for interrupt %u\r\n",
                        irq_num);
                while (1)
                    ;
            }
        }
    }
    return MAKE_SATP(currpid, ppcb->pagetable);   
}

