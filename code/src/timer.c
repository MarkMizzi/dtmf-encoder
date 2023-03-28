#include "timer.h"

#include <platform.h>
#include <system_LPC407x_8x_177x_8x.h>
#include <stddef.h>


/** 
 * Holds the interrupt invoked when the SysTick timer goes off.
 */
static void (*timer_interrupt_isr)(void) = NULL;

/**
 * The handler invoked when the SysTick timer goes off.
 *
 * Since we would like to customize the interrupt handler invoked, and also
 * disable the SysTick mechanism as needed, this handler is effectively a wrapper for invoking
 * #timer_interrupt_isr. 
 * 
 * If the latter is set to `NULL`, nothing happens; the timer interrupt is effectively disabled.
 */
void SysTick_Handler(void) {
	if (timer_interrupt_isr != NULL) {
		timer_interrupt_isr();
	}
}

void timer_enable(void (*timer_callback)(void), float timer_freq) {
	timer_interrupt_isr = NULL;
	SysTick_Config((int)(SystemCoreClock / timer_freq));
	timer_interrupt_isr = timer_callback;
}

void timer_disable(void) {
	timer_interrupt_isr = NULL;
}
