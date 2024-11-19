/**
 * @file     main.c
 */
/* Embedded Xinu, Copyright (C) 2023.  All rights reserved. */

#include <xinu.h>

void testcases(void);

/**
 * Main process.  You can modify this routine to customize what Embedded Xinu
 * does when it starts up.  The default is designed to do something reasonable
 * on all platforms based on the devices and features configured.
 */
process main(void)
{
    kprintf("Hello Xinu World!\r\n");

    testcases();

    return 0;
}