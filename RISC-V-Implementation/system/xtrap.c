#include <xinu.h>

// Found in 3.1.15 Machine Cause Register in the RISC-V Privileged documentation
char *trap_names[] = {
    "Instruction address misaligned",
    "Instruction access fault",
    "Illegal instruction",
    "Breakpoint",
    "Load address misaligned",
    "Load access fault",
    "Store/AMO address misaligned",
    "Store/AMO access fault",
    "Environment call from U-mode",
    "Environment call from S-mode",
    "Reserved",
    "Environment call from M-mode",
    "Instruction page fault",
    "Load page fault",
    "Reserved", 
    "Store/AMO page fault"
};

void xtrap(ulong *frame, ulong cause, ulong address, ulong *pc)
{
    /* If not an interrupt or syscall, fall through to generic exception handler */
    kprintf("\r\n\r\nXINU Exception [%s]\r\n", trap_names[cause]);
    kprintf("Faulting code: 0x%016lX\r\n", pc);

    if (address != 0){
        kprintf("Faulting address: 0x%016lX\r\n", address);
    }

    kprintf("[0x%016lX]  t4:0x%016lX   t5:0x%016lX  t6:0x%016lX\r\n",
            frame + CTX_T4,
            frame[CTX_T4], frame[CTX_T5], frame[CTX_T6]); 
    kprintf("[0x%016lX]  s9:0x%016lX   s10:0x%016lX  s11:0x%016lX  t3:0x%016lX\r\n",
            frame + CTX_S9,
            frame[CTX_S9], frame[CTX_S10], frame[CTX_S11], frame[CTX_T3]); 
    kprintf("[0x%016lX]  s5:0x%016lX   s6:0x%016lX  s7:0x%016lX  s8:0x%016lX\r\n",
            frame + CTX_S5,
            frame[CTX_S5], frame[CTX_S6], frame[CTX_S7], frame[CTX_S8]); 
    kprintf("[0x%016lX]  a7:0x%016lX   s2:0x%016lX  s3:0x%016lX  s4:0x%016lX\r\n",
            frame + CTX_A7,
            frame[CTX_A7], frame[CTX_S2], frame[CTX_S3], frame[CTX_S4]); 
    kprintf("[0x%016lX]  a3:0x%016lX   a4:0x%016lX  a5:0x%016lX  a6:0x%016lX\r\n",
            frame + CTX_A3,
            frame[CTX_A3], frame[CTX_A4], frame[CTX_A5], frame[CTX_A6]);            
    kprintf("[0x%016lX]  s1:0x%016lX   a0:0x%016lX  a1:0x%016lX  a2:0x%016lX\r\n",
            frame + CTX_S1,
            frame[CTX_S1], frame[CTX_A0], frame[CTX_A1], frame[CTX_A2]);
    kprintf("[0x%016lX]  t0:0x%016lX   t2:0x%016lX   t3:0x%016lX   s0:0x%016lX\r\n",
            frame + CTX_T0,
            frame[CTX_T0], frame[CTX_T1], frame[CTX_T2], frame[CTX_S0]);
    kprintf("[0x%016lX]  ra:0x%016lX   sp:0x%016lX   gp:0x%016lX   tp:0x%016lX\r\n",
            frame + CTX_RA,
            frame[CTX_RA], frame[CTX_SP], frame[CTX_GP], frame[CTX_GP]);
    while (1)
        ;                       /* forever */
}