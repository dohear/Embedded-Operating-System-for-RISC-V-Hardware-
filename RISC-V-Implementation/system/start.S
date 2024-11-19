/**
 * @file start.S
 *
 * Initialization code for Embedded Xinu on the RISC-V Virt device.
 *
 * Embedded Xinu, Copyright (C) 2022.  All rights reserved.
 *
 */
#include <riscv.h>

.section .init
	.globl _start

	/* _start:  Entry point of the Xinu kernel.  This will be the very first
	/* byte of the kernel image and on the will be loaded at address 0x80000000.  */

	.func _start
_start:
	/* Continue at reset handler. */
	j reset_handler
	.endfunc

.section .text
	/* reset_handler: Reset handler routine executed to start up the kernel,
	 * when the processor is reset, or (currently) when an unhandled
	 * exception occurs.  */
    .func reset_handler
reset_handler:

	la a1, _end
	li a3, 16384
	add sp, a1, a3

	la a0, _bss
	la t0, _end
	sub a1, t0, a0
	call bzero

	la a4, memheap
	sw sp, 0(a4)

	// Set the previous mode to S-Mode
	li t1, RISCV_MPP_TO_S_MODE
	csrs mstatus, t1

	// Clear all the bits in the exception delegation register
	li t1, RISCV_MDELEG_ALL_S_MODE
	csrc medeleg, t1

	// Delegate ENV calls in U-Mode to be handled in S-Mode
	li t1, RISCV_ENV_UMODE
	csrs medeleg, t1

	// Tell the CPU to handle all interrupts in S-Mode
	li t1, RISCV_MDELEG_ALL_S_MODE
	csrc mideleg, t1
	csrs mideleg, t1

	// Tell the CPU to handle interrupts in S-Mode
	li t1, RISCV_SIE_SSIE
	csrs sie, t1

	li t1, RISCV_SIE_STIE
	csrs sie, t1

	li t1, RISCV_SIE_SEIE
	csrs sie, t1

	// Allow S-Mode to access all of memory
	li t1, RISCV_MAX_ADDR
	csrw pmpaddr0, t1

	li t1, RISCV_ALL_PERM
	csrw pmpcfg0, t1

	sfence.vma zero, zero

	// Allow S mode to access U mode pages
	li t1, RISCV_MSTATUS_SUM
	csrrs x0, mstatus, t1

	// Loads address of the function interrupt in t1
	la t1, interrupt
	// Sets stvec to the address of interrupt
	csrw stvec, t1

	// Turn off all caches but the instruction cache
	li t1, 0x109
	csrw 0x7C1, t1

	// Loads address of the function criticalerr in t1
	la t1, criticalerr
	// Sets stvec to the address of criticalerr
	csrw mtvec, t1

	// Set the program counter to nulluser so when we run mret, it runs nulluser
	la t1, nulluser
	csrw mepc, t1

	li tp, 0
	li gp, 0

	// Jump to null user
	mret
	.endfunc
