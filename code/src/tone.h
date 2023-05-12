#ifndef TONE_H
#define TONE_H

/**
 * \brief Initialises the DAC and creates the Sin Wave LUT.
 */
void tone_init();

/**
 * \brief Attempts to start a new symbol's interrupt. If a symbol is already being played, the symbol is 
 * enqueued to a global queue. The symbol's corresponding character is outputted to the LCD.
 *
 * @param col The column of the symbol whose tone is to be generated.
 * @param row The row of the symbol whose tone is to be generated.
 */
void tone_play_or_enqueue(int row, int col);

#endif // TONE_H
