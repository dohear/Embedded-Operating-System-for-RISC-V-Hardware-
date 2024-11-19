/**
 * @file mmu.S
 * @provides set_satp
 */
 /* Embeddedd Xinu. Copyright (C) 2022. All rights reserved. */


.globl set_satp
set_satp:
	.func set_satp
	sfence.vma zero, zero
	csrw satp, a0
	sfence.vma zero, zero
	ret
	.endfunc

