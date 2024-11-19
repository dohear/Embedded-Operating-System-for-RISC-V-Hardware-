/**
 * @file ns16550.h
 *
 * Contains structs, functions, and constants for the NS16550 UART
 */
/* Embedded Xinu, Copyright (C) 2024.  All rights reserved. */

#ifndef _NS16550_H_
#define _NS16550_H_

#include <stddef.h>

#define UART_BASE  0x2500000L

/**
 * Control and status registers for the 16550 UART.  This structure is
 * mapped directly to the base address for the CSR.
 * See D1_User_Manual, Section 9.2.5 (p.897)
 */
struct ns16550_uart_csreg
{
    volatile unsigned int buffer;	/**< receive buffer (read only)      */
    					/**<  OR transmit hold (write only)  */
    volatile unsigned int ier;		/**< interrupt enable                */
    volatile unsigned int iir;		/**< interrupt ident (read only)     */
    					/**<  OR FIFO control (write only)   */
    volatile unsigned int lcr;		/**< line control                    */
    volatile unsigned int mcr;		/**< modem control                   */
    volatile unsigned int lsr;		/**< line status                     */
    volatile unsigned int msr;		/**< modem status                    */
    volatile unsigned int scr;		/**< scratch                         */
};

/* Alternative names for control and status registers                   */
#define rbr buffer              /**< receive buffer (read only)         */
#define thr buffer              /**< transmit hold (write only)         */
#define fcr iir                 /**< FIFO control (write only)          */
#define dll buffer              /**< divisor latch low byte             */
#define dlh ier                 /**< divisor latch high byte            */

/* UART Bit flags for control and status registers                      */
/* Interrupt enable bits                                                */
#define UART_IER_ERBFI	(1<<0)	/**< Received data interrupt mask       */
#define UART_IER_ETBEI	(1<<1)	/**< Transmitter buffer empty interrupt */
#define UART_IER_ELSI	(1<<2)	/**< Recv line status interrupt mask    */
#define UART_IER_EDSSI	(1<<3)	/**< Modem status interrupt mask        */
#define UART_IER_INT_EN	(1<<4)	/**< RS485 interrupt enable             */
#define UART_IER_PTIME	(1<<7)	/**< Programmable THRE interrupt enable */

/* FIFO control bits */
#define UART_FCR_FIFOE	(1<<0)	/**< Enable in and out hardware FIFOs   */
#define UART_FCR_RFIFOR	(1<<1)	/**< Reset receiver FIFO                */
#define UART_FCR_XFIFOR	(1<<2)	/**< Reset transmit FIFO                */
#define UART_FCR_DMAM	(1<<3)	/**< DMA mode                           */
#define UART_FCR_TRIG0  0x00    /**< RCVR FIFO trigger level one char   */
#define UART_FCR_TRIG1  0x40    /**< RCVR FIFO trigger level 1/4        */
#define UART_FCR_TRIG2  0x80    /**< RCVR FIFO trigger level 2/4        */
#define UART_FCR_TRIG3  0xC0    /**< RCVR FIFO trigger level 3/4        */

/* Line control bits */
#define UART_LCR_STOP	(1<<2)	/**< Parity enable                      */
#define UART_LCR_PEN	(1<<3)	/**< Number of stop bits                */
#define UART_LCR_BC	(1<<6)	/**< Break control bit                  */
#define UART_LCR_DLAB	(1<<7)	/**< Divisor latch access bit           */
#define UART_LCR_8N1    0x03    /**< 8 bits, no parity, 1 stop          */

/* Modem control bits */
#define UART_MCR_DTR	(1<<0)	/**< Data terminal ready                */
#define UART_MCR_RTS	(1<<1)	/**< Request to send                    */
#define UART_MCR_LOOP   (1<<4)	/**< Enable loopback test mode          */
#define UART_MCR_AFCE   (1<<5)	/**< Auto flow control enable           */

/* Line status bits */
#define UART_LSR_DR	(1<<0)	/**< Data ready                         */
#define UART_LSR_OE	(1<<1)	/**< Overrun error                      */
#define UART_LSR_PE	(1<<2)	/**< Parity error                       */
#define UART_LSR_FE	(1<<3)	/**< Framing error                      */
#define UART_LSR_BI	(1<<4)	/**< Break interrupt                    */
#define UART_LSR_THRE	(1<<5)	/**< Transmit-hold-register empty       */
#define UART_LSR_TEMT	(1<<6)	/**< Transmitter empty                  */
#define UART_LSR_FIFOERR	(1<<7)	/**< Rx data error in FIFO      */

/* Modem status bits */
#define UART_MSR_DCTS	(1<<0)	/**< Delta clear to send                */
#define UART_MSR_DDSR	(1<<1)	/**< Delta data set ready               */
#define UART_MSR_TERI	(1<<2)	/**< Trailing edge ring indicator       */
#define UART_MSR_DDCD	(1<<3)	/**< Delta data carrier detect          */
#define UART_MSR_CTS	(1<<4)	/**< Line state of clear to send        */
#define UART_MSR_DSR	(1<<5)	/**< Line state of data set ready       */
#define UART_MSR_RI	(1<<6)	/**< Line state of ring indicator       */
#define UART_MSR_DCD	(1<<7)	/**< Line state of data carrier detect  */

#endif                          /* _NS16550_H_ */
