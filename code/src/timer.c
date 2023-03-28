#include "timer.h"

#include <platform.h>
#include <system_LPC407x_8x_177x_8x.h>
#include <stdbool.h>
#include <stddef.h>

/** Computes the number of invocations of the timer handler needed to emulate a delay of DELAY_S.
 */
#define DELAY_FACTOR(DELAY_S) (((int)(SystemCoreClock * (DELAY_S))) / SysTick_LOAD_RELOAD_Msk)
#define DELAY_REMNDR(DELAY_S) (((int)(SystemCoreClock * (DELAY_S))) % SysTick_LOAD_RELOAD_Msk)
#define DELAY_CYCLES SysTick_LOAD_RELOAD_Msk

static unsigned delay_counter = 0;
static unsigned delay_factor = 0;
static unsigned delay_remainder = 0;
static bool in_delay_remainder = false;

/** 
 * Holds the interrupt invoked when the SysTick timer goes off.
 */
static void (*timer_interrupt_isr)(void) = NULL;

static void (*delayed_callback)(void) = NULL;

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

void delay_callback_handler() {
	if (in_delay_remainder) {
		timer_disable();
		delayed_callback();
	} else if (delay_counter >= delay_factor) {
		in_delay_remainder = true;
	} else {
		delay_counter++;
	}
}

void delay_callback(void (*callback)(void), float delay_s) {
	timer_interrupt_isr = NULL;
	
	delay_counter = 0;
	in_delay_remainder = false;
	delay_factor = DELAY_FACTOR(delay_s);
	delay_remainder = DELAY_REMNDR(delay_s);
	SysTick_Config(delay_factor ? DELAY_CYCLES : delay_remainder);

	delayed_callback = callback;
	timer_interrupt_isr = delay_callback_handler;
}
