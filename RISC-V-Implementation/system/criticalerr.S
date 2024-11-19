/**
 * @file criticalerr.S
 */
/* Embedded Xinu, Copyright (C) 2023.  All rights reserved. */

#include <riscv.h>

.text
    .align 4
.globl criticalerr

/**
 * Entry point for Xinu's unrecoverable error handler (RISC-V version). 
 */
criticalerr:
	.func criticalerr
    csrw mscratch, sp

    li sp, 0x52000000
    addi sp, sp, CONTEXT*-8

    sd t0, CTX_T0*8(sp)
    csrr t0, mscratch
    sd t0, CTX_SP*8(sp)

    sd ra, CTX_RA*8(sp)
    sd gp, CTX_GP*8(sp)
    sd tp, CTX_TP*8(sp)
    sd t1, CTX_T1*8(sp)
    sd t2, CTX_T2*8(sp)
    sd s0, CTX_S0*8(sp)
    sd s1, CTX_S1*8(sp)
    sd a0, CTX_A0*8(sp)
    sd a1, CTX_A1*8(sp)
    sd a2, CTX_A2*8(sp)
    sd a3, CTX_A3*8(sp)
    sd a4, CTX_A4*8(sp)
    sd a5, CTX_A5*8(sp)
    sd a6, CTX_A6*8(sp)
    sd a7, CTX_A7*8(sp)
    sd s2, CTX_S2*8(sp)
    sd s3, CTX_S3*8(sp)
    sd s4, CTX_S4*8(sp)
    sd s5, CTX_S5*8(sp)
    sd s6, CTX_S6*8(sp)
    sd s7, CTX_S7*8(sp)
    sd s8, CTX_S8*8(sp)
    sd s9, CTX_S9*8(sp)
    sd s10, CTX_S10*8(sp)
    sd s11, CTX_S11*8(sp)
    sd t3, CTX_T3*8(sp)
    sd t4, CTX_T4*8(sp)
    sd t5, CTX_T5*8(sp)
    sd t6, CTX_T6*8(sp)

    csrr a1, mcause
    slli a1, a1, 1
    srli a1, a1, 1

    mv a0, sp
    csrr a1, mcause
    csrr a2, mtval
    csrr a3, mepc
    j xtrap
    .endfunc
