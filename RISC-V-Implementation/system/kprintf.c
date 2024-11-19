
/**
 * COSC 3250 - Project 3
 * this file contains the xinu functions to get chars, put/print chars, unget chars and check if there is space to hold a char
 * @author Adam Samuelson & Daniel O'Hear
 * Instructor Dr. Brylow
 * TA-BOT:MAILTO adam.samuelson@marquette.edu daniel.ohear@marquette.edu
 */
#include <xinu.h>

#define UNGETMAX 10             /* Can un-get at most 10 characters. */
static unsigned char ungetArray[UNGETMAX];
unsigned int  bufp = 0;

syscall kgetc()
{
    volatile struct ns16550_uart_csreg *regptr;
    regptr = (struct ns16550_uart_csreg *)UART_BASE;

    //checks the unget buffer for a character and returns it if there is one.
    if (bufp > 0)
        return ungetArray[--bufp];
    int val = 0;
    //check UART line status register, and once there is data ready, gets character c and returns a pointer to it.
    do {
        val = regptr->lsr;
    } while((val & UART_LSR_DR) == 0);
    return regptr->rbr;
}

/**
 * kcheckc - check to see if a character is available.
 * @return true if a character is available, false otherwise.
 */
syscall kcheckc(void)
{
    volatile struct ns16550_uart_csreg *regptr;
    regptr = (struct ns16550_uart_csreg *)UART_BASE;

    //checks if there is a char in the UART line status register or the buffer and returns true or false accordingly 
    if((regptr->lsr & UART_LSR_DR) || (bufp > 0))
        return 1;
    else
        return 0;
}

/**
 * kungetc - put a serial character "back" into a local buffer.
 * @param c character to unget.
 * @return c on success, SYSERR on failure.
 */
syscall kungetc(unsigned char c)
{
    //checks if buffer is empty and if its not the char c is put in the buffer
    if(bufp >= UNGETMAX)
        return SYSERR;
    else{
        ungetArray[bufp] = c;
        bufp++;
    }
    return OK;
}

syscall kputc(uchar c)
{
    volatile struct ns16550_uart_csreg *regptr;
    regptr = (struct ns16550_uart_csreg *)UART_BASE;

    //checks the UART line status register, Once the Transmitter FIFO is empty, returns character c.
    int val = 0;
    do {
        val = regptr->lsr;
    } while ((val & UART_LSR_THRE) == 0);
    regptr->thr = c;
    return c;
}

syscall kprintf(const char *format, ...)
{
    int retval;
    va_list ap;

    va_start(ap, format);
    retval = _doprnt(format, ap, (int (*)(long, long))kputc, 0);
    va_end(ap);
    return retval;
}
