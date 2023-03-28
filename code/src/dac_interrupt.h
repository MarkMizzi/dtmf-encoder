#pragma once

#include <stdbool.h>

#define SYMBOL_LENGTH 1
#define INTERSYMBOL_SPACING ((float)0.5)

/**
 * Attempts to enable the DAC interrupt which generates tone for a given symbol.
 *
 * Two tones cannot be generated at once, so this function first performs an
 * atomic test and set on a global flag that determines whether a DAC interrupt
 * is running. 
 * 
 * If the fetched value of the flag is true, an interrupt is already
 * running, and the function returns false.
 * Otherwise the function can enable the interrupt.
 *
 * The DAC interrupt is triggered on a timer, and so this function uses the
 * timer interface to enable the interrupt.
 *
 * @param col The column of the symbol whose tone is to be generated.
 * @param row The row of the symbol whose tone is to be generated.
 * @return Whether the function call succeeded in enabling the interrupt.
 */
bool dac_interrupt_enable(int col, int row);
