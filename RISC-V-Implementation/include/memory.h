/**
 * @file memory.h
 * Definitions for kernel memory allocator and maintenance.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stddef.h>

/* Other memory data */

extern void *_end;              /* linker provides end of image       */
extern void *_bss;              /* linker provides start of bss       */
extern void *_datas;            /* linker provides start of data      */
extern void *memheap;           /* bottom of heap                     */
extern void *_ctxsws;           /* start of ctxsw                     */
extern void *_ctxswe;           /* end of ctxsw                       */
extern void *_interrupts;       /* start of interrupts                */
extern void *_interrupte;       /* end of interrupts                  */
extern ulong *_kernpgtbl;	/* kernel page table                  */
extern ulong *_kernsp;          /* kernel stack pointer               */

#endif                          /* _MEMORY_H_ */
