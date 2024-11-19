/**
 * @file riscv.h
 * 
 * Contains constants for the RISC-V architecture.  
 * NOTE: This file is included in assembly, thus it can only contain constants
 */
/* Embedded Xinu, Copyright (C) 2024.  All rights reserved. */
#ifndef  _RISCV_H_
#define  _RISCV_H_

#define RISCV_MSTATUS_SUM       (1L<<18)
#define RISCV_MSTATUS_MEI_BIT   (1<<3)  /* IRQs globally disabled on all privilege levels when set to 1. */
#define RISCV_MPP_TO_S_MODE     (1<<11) /* Set the previous mode to S-mode so XINU can switch to S-Mode when completed. */
#define RISCV_SPP_TO_U_MODE     (1<<8)  /* Set the previous mode to U-mode so XINU can switch to U-Mode when completed. */
#define RISCV_SIE_ENABLE        (1<<1)
#define RISCV_SIE_DISABLE       (0<<1)
#define RISCV_MDELEG_ALL_S_MODE 0xFFFFFFFFFFFFFFFF
#define RISCV_ENV_UMODE         (1L<<8)

#define RISCV_SIE_SEIE (1<<9)
#define RISCV_SIE_STIE (1<<5)
#define RISCV_SIE_SSIE (1<<1)

#define RISCV_ENABLE_ALL_SMODE_INTR (RISCV_SIE_SEIE | RISCV_SIE_STIE | RISCV_SIE_SSIE)
#define RISCV_MAX_ADDR 0x3FFFFFFFFFFFFFull
#define RISCV_ALL_PERM 0xF

#define MAXVIRTADDR	0x4000000000    //(1L << 38)
#define INTERRUPTADDR	0x3FFFFFF000    // truncpage((MAXVIRTADDR - PAGE_SIZE))
#define SWAPAREAADDR	0x3FFFFFE000    // truncpage((MAXVIRTADDR - PAGE_SIZE))
#define PROCSTACKADDR	0x3FFFFFD000    // truncpage((MAXVIRTADDR - PAGE_SIZE - PAGE_SIZE))

#define WATCHDOG_CONF   0x00020500B4

#define SSTATUS_S_MODE  (1L<<8)
#define SSTATUS_U_MODE  0x0
#define SSTATUS_PRIV_MODE_BIT   (1L<<8)

#define CONTEXT   36            /**< context record size in words         */
#define ARG_REG_MAX 8

#define CTX_A0 0
#define CTX_A1 1
#define CTX_A2 2
#define CTX_A3 3
#define CTX_A4 4
#define CTX_A5 5
#define CTX_A6 6
#define CTX_A7 7
#define CTX_S0 8
#define CTX_S1 9
#define CTX_S2 10
#define CTX_S3 11
#define CTX_S4 12
#define CTX_S5 13
#define CTX_S6 14
#define CTX_S7 15
#define CTX_S8 16
#define CTX_S9 17
#define CTX_S10 18
#define CTX_S11 19
#define CTX_RA 20
#define CTX_SP 21
#define CTX_PC 22

#define CTX_GP 23
#define CTX_TP 24
#define CTX_T0 25
#define CTX_T1 26
#define CTX_T2 27
#define CTX_T3 28
#define CTX_T4 29
#define CTX_T5 30
#define CTX_T6 31

#define CTX_KERNSATP 32
#define CTX_KERNSP 33

#endif                          /* _RISCV_H_ */
