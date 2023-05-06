/*!
 * \file      timer.h
 * \brief     Controller for a hardware timer module.
 * \copyright ARM University Program &copy; ARM Ltd 2014.
 */
#ifndef TIMER_H
#define TIMER_H
#include <platform.h>
#include <stdint.h>

#define FREQ_HZ_TO_CYCLES(FREQ_HZ) (SystemCoreClock / (FREQ_HZ))

// NOTE: In this case we do a division first, since otherwise PERIOD_MS * SystemCoreClock is prone to overflow.
// There is no loss in precision since 1000 divides SystemCoreClock
#define PERIOD_MS_TO_CYCLES(PERIOD_MS) ((PERIOD_MS) * (SystemCoreClock / 1000U))

#define PERIOD_S_TO_CYCLES(PERIOD_S) ((PERIOD_S) * SystemCoreClock)

/*! \brief Initialises the timer with a specified period.
 *  \param period  Period of the timer tick (in cpu \a cycles).
 */
void timer_init(uint32_t period);

/*! \brief Pass a callback to the API, which is executed during the
 *         interrupt handler.
 *  \param callback  Callback function.
 */
void timer_set_callback(void (*callback)(void), uint32_t period);

void timer_set_callback_delay(void (*callback)(void), uint32_t delay);

/*! \brief Enables the timer operation. */
void timer_enable(void);

/*! \brief Disables the timer. */
void timer_disable(void);

#endif // TIMER_H

// *******************************ARM University Program Copyright � ARM Ltd 2014*************************************   
