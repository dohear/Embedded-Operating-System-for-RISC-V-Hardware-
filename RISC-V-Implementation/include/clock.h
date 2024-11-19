/**
 * @file clock.h
 * Definitions relating to the hardware clock and the Xinu clock interrupt.
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2023.  All rights reserved. */

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <stddef.h>

/**
 * @ingroup timer
 *
 * Timer resolution.  This refers to the maximum number of times that Xinu
 * schedules the timer interrupt per second, @a not to the number of native
 * clock cycles per second.
 */
#define CLKTICKS_PER_SEC  1000

#define QUANTUM 	3       /* clock ticks until preemption */

/**
 * The "preempt" quantum indicates when the scheduler should preempt.
 * For example, if the QUANTUM is 3, then after 3 clock ticks, the
 * scheduler will preempt and then reset the value of the preempt
 * quantum to QUANTUM.
 */
extern volatile ulong preempt;

/**
 * clkticks and clktime keep track of information about the clock.
 */
extern volatile ulong clkticks;
extern volatile ulong clktime;

/* Clock function prototypes. */
void clkinit(void);
interrupt clkhandler(void);

#endif                          /* _CLOCK_H_ */
