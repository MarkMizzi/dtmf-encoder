#ifndef TONE_H
#define TONE_H

/**
 * \brief Initialises the DAC and creates the Sin Wave LUT.
 */
void tone_init(void);

/**
 * \brief Attempts to start an interrupt to generate a tone, and displays symbol on the LCD.
 * 
 * If a tone is already being generated, the symbol corresponding to the tone is 
 * enqueued to a global queue. The symbol's corresponding character is displayed on the LCD.
 *
 * \param col The column of the symbol whose tone is to be generated.
 * \param row The row of the symbol whose tone is to be generated.
 */
void tone_play_or_enqueue(int row, int col);

#endif // TONE_H
