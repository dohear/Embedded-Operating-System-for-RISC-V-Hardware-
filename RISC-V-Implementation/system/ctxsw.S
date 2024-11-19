/**
 * COSC 3250 - Project 4
 * This process assists with context switching between processes
 * @author Adam Samuelson & Daniel O'Hear
 * Instructor Dr. Brylow
 * TA-BOT:MAILTO adam.samuelson@marquette.edu daniel.ohear@marquette.edu
 */
/**
 * @file     ctxsw.s
 * @provides ctxsw
 *
 * COSC 3250 / COEN 4820 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2023.  All rights reserved. */

#include <riscv.h>

.section .ctxswsec
	.align 4
	.globl	ctxsw


/**
 * @fn void ctxsw(&oldstack, &newstack)
 *
 * Switch context (values in registers) to another process, saving the
 * current processes information. This function will not return as normally
 * thought as it will load in the program counter for a different process and
 * jump to that location and begin executing code.
 *
 * @param  &oldstack address of outgoing stack save area
 * @param  &newstack address of incoming stack save area
 * @return special case -- see above
 */
	.func ctxsw
ctxsw:



// TODO: Save process state, including at least the
//  callee-save ("non-volatile") registers.

    // moves stack pointer to make space
    addi sp, sp, -32*8

    sd s0, (a0)

    //stores all registers to the stack
    sd  ra, CTX_PC*8(sp)
    sd  x1, CTX_RA*8(sp)
    sd  x2, CTX_SP*8(sp)
    sd  x3, CTX_GP*8(sp) 
    sd  x4, CTX_TP*8(sp)
    sd  x5, CTX_T0*8(sp)
    sd  x6, CTX_T1*8(sp)
    sd  x7, CTX_T2*8(sp)
    sd  s0, CTX_S0*8(sp)
    sd  x9, CTX_S1*8(sp)
    sd  x10, CTX_A0*8(sp)
    sd  x11, CTX_A1*8(sp)
    sd  x12, CTX_A2*8(sp)
    sd  x13, CTX_A3*8(sp)
    sd  x14, CTX_A4*8(sp)
    sd  x15, CTX_A5*8(sp)
    sd  x16, CTX_A6*8(sp)
    sd  x17, CTX_A7*8(sp)
    sd  x18, CTX_S2*8(sp)
    sd  x19, CTX_S3*8(sp)
    sd  x20, CTX_S4*8(sp)
    sd  x21, CTX_S5*8(sp)
    sd  x22, CTX_S6*8(sp)
    sd  x23, CTX_S7*8(sp)
    sd  x24, CTX_S8*8(sp)
    sd  x25, CTX_S9*8(sp)
    sd  x26, CTX_S10*8(sp)
    sd  x27, CTX_S11*8(sp)
    sd  x28, CTX_T3*8(sp)
    sd  x29, CTX_T4*8(sp)
    sd  x30, CTX_T5*8(sp)
    sd  x31, CTX_T6*8(sp)

    //stores a0 to the stack pointer
    sd sp, (a0)

// TODO: Restore process state, including at least the
//  callee-save ("non-volatile") registers.
//  These call and restore segments must match the register data
//  layout you choose in create().

    //this loads a1 in the stack pointer
    ld  sp, (a1)

    //this loads all of the registers into the stack
    ld  x1, CTX_RA*8(sp)
    ld  x2, CTX_SP*8(sp)
    ld  x3, CTX_GP*8(sp) 
    ld  x4, CTX_TP*8(sp)
    ld  x5, CTX_T0*8(sp)
    ld  x6, CTX_T1*8(sp)
    ld  x7, CTX_T2*8(sp)
    ld  s0, CTX_S0*8(sp)
    ld  x9, CTX_S1*8(sp)
    ld  x10, CTX_A0*8(sp)
    ld  x11, CTX_A1*8(sp)
    ld  x12, CTX_A2*8(sp)
    ld  x13, CTX_A3*8(sp)
    ld  x14, CTX_A4*8(sp)
    ld  x15, CTX_A5*8(sp)
    ld  x16, CTX_A6*8(sp)
    ld  x17, CTX_A7*8(sp)
    ld  x18, CTX_S2*8(sp)
    ld  x19, CTX_S3*8(sp)
    ld  x20, CTX_S4*8(sp)
    ld  x21, CTX_S5*8(sp)
    ld  x22, CTX_S6*8(sp)
    ld  x23, CTX_S7*8(sp)
    ld  x24, CTX_S8*8(sp)
    ld  x25, CTX_S9*8(sp)
    ld  x26, CTX_S10*8(sp)
    ld  x27, CTX_S11*8(sp)
    ld  x28, CTX_T3*8(sp)
    ld  x29, CTX_T4*8(sp)
    ld  x30, CTX_T5*8(sp)
    ld  x31, CTX_T6*8(sp)
    ld  t1, CTX_A1*8(sp)
    ld  t0, CTX_PC*8(sp)
    //this moves the stack pointer back to the top of the stack
    addi sp, sp, 32*8
    //this stores the stack pointer in a1
    sd sp, (a1)    
    mv a1, t1



    
// TODO: Jump to next function.
//	Note: there are two important values at play here --
//	the address of the process's function that we want to end up in
//	once we leave this ctxsw(), and the value we want to have in the
//	return address register.  Those are almost always the same value, except
//	for the very first time we ctxsw() to a new process, when they
//	are different.  See create() for more details.
	// TODO: Jump to next function.

//load inreturn adress of function


// check if this is first ctxsw call


//jump to the next function and store the addresss in the return address register then exit the code




//for cases where this is the first call fo ctxsw set the return address to 0 and jump to the next function

 

// Note: This ret line immediately returns from whence we came.  This
//	is here as a placeholder so that you don't crash your O/S when
//	you call ctxsw() before it is complete.  But, this line of code
//	is not actually a part of a completed solution.

// ASSIGNMENT 5 TODO:
// Note: the following code is for the next assignment.  DO NOT uncomment it until you believe your trap handler is working in assignment
// Determine which mode we privilege mode the process should be in
//	ld t0, CTX_PC*8(**REGISTER THAT HOLDS PROCESS CONTEXT**)



beq t0, ra, switch

csrw satp, a2
sfence.vma zero, zero
mv a2, t2

li t4, SSTATUS_S_MODE
csrc sstatus, t4
csrw sepc, t0
sret
switch:
ret

	.end ctxsw
