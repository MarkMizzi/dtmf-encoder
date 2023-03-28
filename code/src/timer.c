#include "timer.h"

#include <platform.h>
#include <system_LPC407x_8x_177x_8x.h>
#include <stdbool.h>
#include <stddef.h>

/** 
 * Maximum SysTick interval in clock cycles.
 */
#define DELAY_CYCLES SysTick_LOAD_RELOAD_Msk

/** 
 * Computes the minimum number of timer invocations required for \p DELAY_S to be emulated (minus 1).
 *
 * We need to compute this as the SysTick interval is at most `DELAY_CYCLES` cycles.
 *
 * @param DELAY_S Delay in seconds.
 */
#define DELAY_FACTOR(DELAY_S) (((int)(SystemCoreClock * (DELAY_S))) / DELAY_CYCLES)

/** 
 * Remainder obtained by dividing a delay (expressed in system clock cycles) by `DELAY_CYCLES`.
 *
 * Used to compute remaining cycles after SysTick has been invoked DELAY_FACTOR() times with 
 * the maximum interval of `DELAY_CYCLES` cycles.
 *
 * @param DELAY_S Delay in seconds.
 */
#define DELAY_REMNDR(DELAY_S) (((int)(SystemCoreClock * (DELAY_S))) % DELAY_CYCLES)

/**
 * Counter used to count up to DELAY_FACTOR() when emulating a delay.
 */
static unsigned delay_counter = 0;
/**
 * Stores computed DELAY_FACTOR() for a delayed callback which is currently enabled.
 */
static unsigned delay_factor = 0;
/**
 * Stores computed DELAY_REMNDR() for a delayed callback which is currently enabled.
 */
static unsigned delay_remainder = 0;
/**
 * Flag used to determine whether the timer has already gone off DELAY_FACTOR() times when 
 * emulating a delay.
 */
static bool in_delay_remainder = false;

/** 
 * Holds the interrupt invoked when the SysTick timer goes off.
 */
static void (*timer_interrupt_isr)(void) = NULL;

/**
 * Holds a callback which will be called after a certain delay.
 *
 * Such delayed callbacks are enabled using delay_callback()
 */
static void (*delayed_callback)(void) = NULL;

/**
 * Interrupt handler executed by SysTick when a delayed callback is enabled.
 *
 * This updates state variables related to the delayed callback, or invokes it if
 * an amount of time equal to the delay has passed.
 */
static void delay_callback_handler(void);

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

static void delay_callback_handler(void) {
	if (in_delay_remainder) {
		// NOTE: important to disable before callback, as callback may re-enable timer.
		timer_disable();
		delayed_callback();
	} else if (delay_counter >= delay_factor) {
		// we've waited for most of the delay, but the remainder is left.
		in_delay_remainder = true;
	} else {
		delay_counter++;
	}
}

void delay_callback(void (*callback)(void), float delay_s) {
	timer_interrupt_isr = NULL;
	
	delay_counter = 0;
	delay_factor = DELAY_FACTOR(delay_s);
	delay_remainder = DELAY_REMNDR(delay_s);
	// if delay_factor == 0, delay_s*SystemCoreClock is smaller than DELAY_CYCLES.
	// But then we can simply emulate the delay using one invocation of the timer interrupt.
	in_delay_remainder = !delay_factor;
	SysTick_Config(delay_factor ? DELAY_CYCLES : delay_remainder);

	delayed_callback = callback;
	timer_interrupt_isr = delay_callback_handler;
}
