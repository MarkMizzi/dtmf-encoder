/*!
 * \file      timer.h
 * \brief     Controller for a hardware timer module.
 * \copyright ARM University Program &copy; ARM Ltd 2014.
 */
#ifndef TIMER_H
#define TIMER_H
#include <platform.h>
#include <stdint.h>

/*! \brief Converts a frequency in Hz to a period in timer cycles.
 *  \param FREQ_HZ Frequency in Hz to convert.
 */
#define FREQ_HZ_TO_CYCLES(FREQ_HZ) (SystemCoreClock / (FREQ_HZ))

/*! \brief Converts a period in milliseconds to a period in timer cycles.
 * 
 * Note that it is important that the division precedes the multiplication, as otherwise the
 * multiplication will be prone to overflow with the large periods used as arguments to this macro.
 * There is no rounding error, as 1000 divides #SystemCoreClock.
 *
 *  \param PEROD_MS Period in milliseconds to convert.
 */
#define PERIOD_MS_TO_CYCLES(PERIOD_MS) ((PERIOD_MS) * (SystemCoreClock / 1000U))

/*! \brief Converts a period in seconds to a period in timer cycles.
 *  \param PEROD_S Period in seconds to convert.
 */
#define PERIOD_S_TO_CYCLES(PERIOD_S) ((PERIOD_S) * SystemCoreClock)

/*! \brief Initialises the timer with a specified period.
 *  \param period  Period of the timer tick (in cpu \a cycles).
 */
void timer_init(uint32_t period);

/*! \brief Pass a callback to the API, which is executed during the
 *         interrupt handler.
 *  \param callback  Callback function.
 *  \param period Period (in timer cycles) that determines frequency of the timer interrupt.
 */
void timer_set_callback(void (*callback)(void), uint32_t period);

/*! \brief Pass a callback to the API, which is executed after a delay.
 *  \param delay The delay (in timer cycles) after which the callback is executed.
 */
void timer_set_callback_delay(void (*callback)(void), uint32_t delay);

/*! \brief Enables the timer operation. */
void timer_enable(void);

/*! \brief Disables the timer. */
void timer_disable(void);

#endif // TIMER_H

// *******************************ARM University Program Copyright � ARM Ltd 2014*************************************   
