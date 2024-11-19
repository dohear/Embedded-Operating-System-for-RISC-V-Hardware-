/**
 * @file     clkinit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/** @ingroup timer
 *
 * Number of timer interrupts that have occurred since ::clktime was
 * incremented.  When ::clkticks reaches ::CLKTICKS_PER_SEC, ::clktime is
 * incremented again and ::clkticks is reset to 0.
 */
volatile ulong clkticks;

/** @ingroup timer
 * Number of seconds that have elapsed since the system booted.  */
volatile ulong clktime;

#if PREEMPT
volatile ulong preempt;
#endif

/**
 * @ingroup timer
 *
 * Initialize the clock and sleep queue.  This function is called at startup.
 */
void clkinit(void)
{
    volatile struct timer *t = (volatile struct timer *)TIMER_BASE;

#if PREEMPT
    preempt = QUANTUM;
#endif

    clkticks = 0;
    clktime = 0;

    // First, configure the timer hardware for T0 periodic interrupt at 1KHz.
    
    // This register is the INTERVAL value of the timer
    // aka how many cycles of the timer hardware between timer interrupts
    t->t0_intv = TIMER_INTV_1KHZ;

    // This is based on the example in the documentation, for a 1mS delay
    // https://dl.linux-sunxi.org/D1/D1_User_Manual_V0.1_Draft_Version.pdf
    // pp. 176-183
    // Set TIMER0 mode to periodic, 24 MHz oscillator source, prescaler 2
    t->t0_ctrl = TMR0_MODE_PERIODIC | TMR0_CLK_SRC_OSC24M | TIMER_PRES_1KHZ;
    platform.clkfreq = 24000000;
    // Reload the TIMER0
    t->t0_ctrl |= TMR0_RELOAD;
    while ((t->t0_ctrl) & TMR0_RELOAD)
      {
	// Spin, waiting for the Reload bit to become 0
      }
    t->t0_ctrl |= TMR0_EN; // Enable the timer

    // Enable TIMER0 interrupt
    t->irq_en = TMR0_IRQ_EN;


    // Second, program platform interrupt controller for TIMER0 source.
    
    // PLIC interrupt priority for TIMER0
    uint *reg = (uint *)(PLIC_BASE + TIMER0_IRQVEC);
    *reg = TIMER0_IRQPRIO;

    // Enable the TIMER0 interrupt for supervisor mode
    uint *timer_irq_enable_reg = 
	    (uint *)(PLIC_BASE + PLIC_SIE_REGN + 0x4 * (TIMER0_INT/32));
    *timer_irq_enable_reg |= (1 << (TIMER0_INT % 32));

    kprintf("Time base %dHz, Clock ticks at %dHz\r\n",
            platform.clkfreq, CLKTICKS_PER_SEC);

    /* register clock interrupt */
    interruptVector[IRQ_TIMER] = clkhandler;
}
