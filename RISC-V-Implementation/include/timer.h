#ifndef _TIMER_H_
#define _TIMER_H_

#include <stddef.h>
#include <stdarg.h>

struct timer
{
  volatile uint irq_en;            /* TMR_IRQ_EN  0x0000 Timer IRQ Enable  */
  volatile uint irq_sta;           /* TMR_IRQ_STA 0x0004 Timer Status      */
  volatile uint blank0;            /*             0x0008                   */
  volatile uint blank1;            /*             0x000C                   */
  volatile uint t0_ctrl;           /* TMR0_CTRL   0x0010 T0 Control        */
  volatile uint t0_intv;           /* TMR0_INTV   0x0014 T0 Internal Value */
  volatile uint t0_curv;           /* TMR0_CURV   0x0018 T0 Current Value  */
  volatile uint blank2;            /*             0x001C                   */
  volatile uint t1_ctrl;           /* TMR1_CTRL   0x0020 T1 Control        */
  volatile uint t1_intv;           /* TMR1_INTV   0x0024 T1 Internal Value */
  volatile uint t1_curv;           /* TMR1_CURV   0x0028 T1 Current Value  */
};

#define TIMER_BASE	0x02050000UL	/* Start of timer registers */

					/* PLIC interrupt sources */
#define TIMER0_INT	75UL		/* Timer0 interrupt request number */
#define TIMER1_INT	76UL		/* Timer1 interrupt request number */
#define TIMER0_IRQVEC	0x012CUL
#define TIMER1_IRQVEC	0x0130UL

#define TIMER0_IRQPRIO	31UL		/* Highest of 32 priorities */

/* TMR_IRQ_EN  0x0000 Timer IRQ Enable bits.  Default = 0x00000000 */
#define TMR1_IRQ_EN     (1<<1)     /* Timer1 Interrupt enable (1=enable)   */
#define TMR0_IRQ_EN     (1<<0)     /* Timer0 Interrupt enable (1=enable)   */

/* TMR_IRQ_STA 0x0004 Timer Status bits.      Default = 0x00000000 */
#define TMR1_IRQ_PEND   (1<<1)     /* IRQ pending Timer1 (Write 1 to clear)*/
#define TMR0_IRQ_PEND   (1<<0)     /* IRQ pending Timer0 (Write 1 to clear)*/

/* TMR0_CTRL   0x0010 T0 Control bits.        Default = 0x00000004 */
#define TMR0_MODE_PERIODIC (0<<7)  /* 0=periodic, 1=single count           */
#define TMR0_MODE_SINGLE   (1<<7)  /* 0=periodic, 1=single count           */
#define TMR0_CLK_PRES_1   (0<<4)   /* Prescaler = 1                        */
#define TMR0_CLK_PRES_2   (1<<4)   /* Prescaler = 2                        */
#define TMR0_CLK_PRES_4   (2<<4)   /* Prescaler = 4                        */
#define TMR0_CLK_PRES_8   (3<<4)   /* Prescaler = 8                        */
#define TMR0_CLK_PRES_16  (4<<4)   /* Prescaler = 16                       */
#define TMR0_CLK_PRES_32  (5<<4)   /* Prescaler = 32                       */
#define TMR0_CLK_PRES_64  (6<<4)   /* Prescaler = 64                       */
#define TMR0_CLK_PRES_128 (7<<4)   /* Prescaler = 128                      */
#define TMR0_CLK_SRC_LOSC   (0<<2) /* Clock source LOSC                    */
#define TMR0_CLK_SRC_OSC24M (1<<2) /* Clock source 24MHz Oscillator        */
#define TMR0_RELOAD     (1<<1)     /* Write 1 to reload T0 interval value  */
#define TMR0_EN         (1<<0)     /* T0 enable.  0=pause, 1=start         */


/* For 1ms clock timer */
#define TIMER_INTV_1KHZ   12000    /* (12000 / 24MHZ) * 2 = 1KHz (1 mS)    */
#define TIMER_PRES_1KHZ   TMR0_CLK_PRES_2


#endif                          /* _TIMER_H_ */
