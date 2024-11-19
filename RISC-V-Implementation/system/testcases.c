/**
 * @file testcases.c
 * @provides testcases
 *
 *
 * Modified by:	
 *
 * TA-BOT:MAILTO 
 *
 */
/* Embedded XINU, Copyright (C) 2023.  All rights reserved. */

#include <xinu.h>

/* Here is a visual representation of the page tables createFakeTable makes.
 * This will allow you to test your printPageTable function without having paging
 * completely working.
 * NOTE: You must have lines 57-58 in initialize.c uncommented for this to work!
 * ┌──────────┐                  ┌──────────┐                ┌───────────┐
 * │          │                  │          │                │           │
 * │          │                  │          │                ├───────────┤
 * |		  |					 |			|				 |  0x8000	 |
 * |──────────|                  |──────────|                ├───────────┤
 * │          ├─────────────────►│          ├───────────────►│           │
 * │──────────│                  |──────────|                ├───────────┤
 * |		  |					 |			|				 |  0x4000	 |
 * │          │                  │          │                ├───────────┤
 * │          │                  │          │                │           │
 * │          │                  └──────────┘                ├───────────┤
 * |		  |					            				 |  0x1000	 |
 * │          │                                              ├───────────┤
 * │          │                                              │           │
 * │          │                                              └───────────┘
 * └──────────┘
 */

int test_method(){
	int x = 4;
	int y = 7;
	kprintf("I test things");
	kprintf("does this work?");
	user_yield();
	return 0;
}
pgtbl createFakeTable(void){
	pgtbl root = pgalloc();
	pgtbl lvl1 = pgalloc();
	pgtbl lvl0 = pgalloc();

	volatile ulong *pte = &(root[5]);
	*pte = PA2PTE(lvl1) | PTE_V;

	ulong *lvl1pte = &(lvl1[145]);
	*lvl1pte = PA2PTE(lvl0) | PTE_V;

	ulong *lvl0pte = &(lvl0[343]);
	*lvl0pte = PA2PTE(0x1000) | PTE_W | PTE_R | PTE_V;

	ulong *lvl0pte1 = &(lvl0[120]);
	*lvl0pte1 = PA2PTE(0x4000) | PTE_X | PTE_R | PTE_V;

	ulong *lvl0pte2 = &(lvl0[45]);
	*lvl0pte2 = PA2PTE(0x8000) | PTE_X | PTE_R | PTE_V;

	return root;
}

void printPageTable(pgtbl pagetable)
{
	/*
	* TODO: Write a function that prints out the page table.
	* Your function should print out all *valid* page table entries in the page table
	* If any of the entires are a link (if the Read/Write/Execute bits aren't set), recursively print that page table
	* Otherwise if it's a leaf, print the page table entry and the physical address is maps to. 
	*/
    
	int i = 0;
	for (i = 0; i < 512; i++){
		if((pagetable[i] & PTE_R) ||(pagetable[i] & PTE_W) || (pagetable[i] & PTE_X)){
			kprintf("leaf: %d\r\n", i);
		}
		else if((pagetable[i] & PTE_V) == PTE_V){
			kprintf("not leaf: %d\r\n", i);
			printPageTable(PTE2PA(pagetable[i]));
		}
	}

}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
	uchar c;
	ulong curr;

	kprintf("===TEST BEGIN===\r\n");

	// TODO: Test your operating system!

	c = kgetc();
	switch (c)
	{
		case '0':
			// TODO: Write a testcase that creates a user process and prints out it's page table
			curr = create((void *)test_method, INITSTK, 100, "testmethod", 0);
			ready(curr, RESCHED_NO);
			pcb *currpcb = &proctab[curr];
			printPageTable(currpcb->pagetable);
			break;
		case '1':
			// TODO: Write a testcase that demonstrates a user process cannot access certain areas of memory
			break;
		case '2':
			// TODO: Write a testcase that demonstrates a user process can read kernel variables but cannot write to them
			break;
		case '3':
			// TODO: Extra credit! Add handling in xtrap to detect and print out a Null Pointer Exception.  Write a testcase that demonstrates your OS can detect a Null Pointer Exception.
			break;
		case '4':
			printPageTable(createFakeTable());
			break;
		default:
			break;
	}

	kprintf("\r\n===TEST END===\r\n");
	return;
}
