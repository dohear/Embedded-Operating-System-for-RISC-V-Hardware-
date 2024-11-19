/**
 * @file syscall_dispatch.c
 * Translate user_* functions to system calls, dispatch to proper sc_*
 * functions, and complete.
 */
/* Embedded Xinu, Copyright (C) 2009, 2020.  All rights reserved. */

#include <xinu.h>

/* for syscall argument parsing */
#define SCARG(type, args)  (type)(*args++)

/* macro to make user->kernel syscalls */
#define SYSCALL(num) int status; \
    asm("li a7, %0" : : "i"(SYSCALL_##num)); \
    asm("ecall"); \
	asm("mv %0, a0" : "=r"(status)); \
	return status;


/* syscall wrapper prototypes */
syscall sc_none(ulong *);
syscall sc_yield(ulong *);
syscall sc_getc(ulong *);
syscall sc_putc(ulong *);
syscall sc_kill(ulong *);

/* table for determining how to call syscalls */
const struct syscall_info syscall_table[] = {
    { 5, (void *)sc_none },     /* SYSCALL_NONE      = 0  */
    { 0, (void *)sc_yield },    /* SYSCALL_YIELD     = 1  */
    { 1, (void *)sc_none },     /* SYSCALL_SLEEP     = 2  */
    { 0, (void *)sc_kill },     /* SYSCALL_KILL      = 3  */
    { 2, (void *)sc_none },     /* SYSCALL_OPEN      = 4  */
    { 1, (void *)sc_none },     /* SYSCALL_CLOSE     = 5  */
    { 3, (void *)sc_none },     /* SYSCALL_READ      = 6  */
    { 3, (void *)sc_none },     /* SYSCALL_WRITE     = 7  */
    { 1, (void *)sc_getc },     /* SYSCALL_GETC      = 8  */
    { 2, (void *)sc_putc },     /* SYSCALL_PUTC      = 9  */
    { 2, (void *)sc_none },     /* SYSCALL_SEEK      = 10 */
    { 4, (void *)sc_none },     /* SYSCALL_CONTROL   = 11 */
    { 1, (void *)sc_none },     /* SYSCALL_GETDEV    = 12 */
    { 4, (void *)sc_none },   /* SYSCALL_CREATE    = 13 */
    { 2, (void *)sc_none },     /* SYSCALL_JOIN      = 14 */
    { 1, (void *)sc_none },     /* SYSCALL_LOCK      = 15 */
    { 1, (void *)sc_none },   /* SYSCALL_UNLOCK    = 16 */
};

int nsyscall = sizeof(syscall_table) / sizeof(struct syscall_info);

/**
 * Syscall dispatch routine.  Given a syscall code and pointer to
 * arguments, change execution to function.  Otherwise, generate error
 * saying no such syscall.
 * @param code syscall code to execute
 * @param args pointer to arguments for syscall
 */
int syscall_dispatch(int code, ulong *args)
{
    if (0 <= code && code < nsyscall)
    {
        return (*syscall_table[code].handler) (args);
    }
    kprintf("ERROR: unknown syscall %d!\r\n", code);
    return SYSERR;
}

/**
 * syscall wrapper for *no* function, just a test/demo.
 * @param args expands to: none.
 */
syscall sc_none(ulong *args)
{
    return OK;
}

syscall user_none(void)
{
    SYSCALL(NONE);
}

/**
 * syscall wrapper for yield().
 * @param args expands to: none.
 */
syscall sc_yield(ulong *args)
{
    return resched();
}

syscall user_yield(void)
{
    SYSCALL(YIELD);
}

/**
 * syscall wrapper for getc().
 * @param args expands to: int descrp
 */
syscall sc_getc(ulong *args)
{
    int descrp = SCARG(int, args);
    if (0 == descrp)
    {
        return kgetc();
    }
    return SYSERR;
}

syscall user_getc(int descrp)
{
    SYSCALL(GETC);
}

/**
 * syscall wrapper for putc().
 * @param args expands to: int descrp, char character
 */
syscall sc_putc(ulong *args)
{
    int descrp = SCARG(int, args);
    char character = SCARG(char, args);

    if (0 == descrp)
        return kputc(character);
    return SYSERR;
}

syscall user_putc(int descrp, char character)
{
    SYSCALL(PUTC);
}

/**
 * syscall wrapper for kill().
 * @param args expands to: none
 */
syscall sc_kill(ulong *args)
{
	return kill(currpid);
}

syscall user_kill(void)
{
    SYSCALL(KILL);
}
