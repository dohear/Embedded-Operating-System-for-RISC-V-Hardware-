/**
 * @file platforminit.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2020.  All rights reserved. */

#include <xinu.h>


/* End of kernel (used for sanity check)  */
extern void *_end;

/**
 * Initializes platform specific information for the QEMU RISC-V Virt
 * @return OK
 */
int platforminit(void)
{
    volatile struct ns16550_uart_csreg *regptr;

    // Set the platform information
    strlcpy(platform.family, "Allwinner D1", PLT_STRMAX);
    strlcpy(platform.type, "Nezha", PLT_STRMAX);
    strlcpy(platform.manufacturer, "Sispeed", PLT_STRMAX);
    strlcpy(platform.extensions, "N/A", PLT_STRMAX);
    platform.revision = 1;

    platform.minaddr = (void *)0x0;
    platform.maxaddr = (void *)0x78FFFFFFUL;

    regptr = (struct ns16550_uart_csreg *)UART_BASE;

    regptr->ier = 0;

    /* Enable UART FIFOs, clear and set interrupt trigger level       */
    regptr->fcr = UART_FCR_RFIFOR | UART_FCR_XFIFOR;
    regptr->fcr = UART_FCR_FIFOE;

    return OK;
}
