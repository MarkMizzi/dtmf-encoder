#pragma once
/**
 * Enables a timer interrupt which triggers \p timer_freq times a second,
 * and invokes \p timer_callback when it is triggered.
 *
 * Internally, the SysTick mechanism is used.
 *
 * @param timer_callback The interrupt handler invoked when the timer goes off.
 * @param timer_freq The frequency with which the timer goes off.
 */
void timer_enable(void (*timer_callback)(void), float timer_freq);

/** Disables any currently registered timer interrupts enabled with timer_enable()
 */
void timer_disable(void);
