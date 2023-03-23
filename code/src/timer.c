#include "timer.h"

#include <platform.h>
#include <system_LPC407x_8x_177x_8x.h>
#include <stddef.h>

static void (*timer_interrupt_isr)(void) = NULL;

void SysTick_Handler(void) {
	if (timer_interrupt_isr != NULL) {
		timer_interrupt_isr();
	}
}

void timer_enable(void (*timer_callback)(void), unsigned timer_freq) {
	timer_interrupt_isr = NULL;
	SysTick_Config(SystemCoreClock / timer_freq);
	timer_interrupt_isr = timer_callback;
}

void timer_disable(void) {
	timer_interrupt_isr = NULL;
}
