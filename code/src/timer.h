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

/**
 * Invokes \p callback after a delay of \p delay_s seconds.
 *
 * @param callback The callback to be invoked after a delay.
 * @param delay_s Delay in seconds.
 */
void delay_callback(void (*callback)(void), float delay_s);
