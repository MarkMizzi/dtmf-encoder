#include <platform.h>
#include <math.h>
#include <dac.h>
#include <timer.h>
#include <gpio.h>
#include <stdbool.h>
#include <limits.h>
#include "tone.h"
#include "delay.h"
#include "dtmf_symbols.h"
#include "queue.h"
#include "lcd.h"

/*! \brief Multiplier for the sampling rate. The size of #sine_table is the sampling rate multiplied by this number.
 * 
 * Having a #sine_table with more samples than necessary for the tone generation is beneficial for more accuracy when
 * computing the lower frequency component of the tone.
 */
#define RATE_MULTIPLIER 32

/*! \brief Size of (number of samples in) #sine_table
*/
#define NUM_STEPS 128

/**
 * Macro function for computing the value of lower frequency sine wave component of a tone.
 *
 * @param BASEFREQ The frequency of the other sine wave component of the tone being produced.
 *                 This partially determines the sampling frequency.
 * @param FREQ The frequency of the sine wave component being computed.
 * @param IDX The index of the sample being computed.
 */
#define SIN(BASEFREQ, FREQ, IDX)                            \
    ((sine_table[((FREQ) * (IDX) / (BASEFREQ)) & (NUM_STEPS-1)] +     \
      sine_table[-(-(FREQ) * (IDX) / (BASEFREQ)) & (NUM_STEPS-1)]) >> \
     1)

/**
 * Macro function which computes a sample in a DTMF tone.
 * 
 * @param F1 The higher of the two frequency components of the tone.
 * @param F2 The lower of the two frequency components of the tone.
 * @param IDX The index of the sample being computed.
 */
#define SIN_ADD(F1, F2, IDX) \
    ((sine_table[(IDX) & (NUM_STEPS-1)] + SIN((F1), (F2), (IDX))) >> 1)

/** 
 * \brief A flag which keeps track of whether a DAC interrupt is enabled or not
 * (i.e. whether a tone is being generated).
 */
int dac_interrupt_flag = 0;

/**
 * \brief A global variable that keeps track of the current sample index in 
 * between invocations of the DAC interrupt handler.
 *
 * The DAC interrupt handler is invoked on a timer to update the DAC (by setting it to
 * output the next sample of a DTMF tone).
 *
 * Its state is torn down in between invocations, so this global stores the information
 * needed to continue producing the tone.
 */
static int sample_index = 0;

/**
 * \brief Attempts to enable the DAC interrupt which generates tone for a given symbol.
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

/**
 * This function pops the next symbol off the global queue, and calls
 * dac_interrupt_enable_unsafe() to start generating its tone.
 *
 * Does nothing if the queue is empty.
 *
 * This is used by the DAC interrupt handler ONLY, and hence it does not use the
 * safe version of dac_interrupt_enable(), since the DAC interrupt cannot be pre-empted,
 * and we can assume that #dac_interrupt_flag is already set.
 * It is not safe to use in normal code.
 */
static void pop_and_dac_interrupt_enable(void);

/** Unsafe version of dac_interrupt_enable().
 *
 * Bypasses the atomic test-and-set operation done by dac_interrupt_enable().
 * This is fine when the function is called inside a DAC interrupt, as this cannot be
 * pre-empted, and we already know that #dac_interrupt_flag is set in this case.
 *
 * @param col The column of the symbol whose tone is to be generated.
 * @param row The row of the symbol whose tone is to be generated.
 */
static void dac_interrupt_enable_unsafe(int col, int row);

/**
 * Disable any DAC interrupt which is currently in progress.
 *
 * Sets the global DAC interrupt flag to false, 
 * and disables the timer interrupt.
 */
static void dac_interrupt_disable(void);

/*! \brief Approximation of pi used to generate the sine look-up table.
 */
#define PI 3.1415927

/*! \brief Sine look-up table used to generate tone samples.
 *
 * This is initialized at program start-up.
 */
static int sine_table[NUM_STEPS];

/**
 * \brief Generic function which contains the implementation of tone generation/output.
 * 
 * This function is specialized for each DTMF tone using TIMER_CALLBACK_ISR() macros.
 * 
 * @param base_freq the base frequency (the higher frequency)
 * @param freq the second frequency (the lower frequency)
 */
static void timer_callback_isr(unsigned base_freq, unsigned freq);

/**
 * \brief Initializes #sine_table with #NUM_STEPS samples from one period of the sine wave.
 */
static void sinewave_init(void);

void tone_init(void) {
	dac_init();
	sinewave_init();
	
	//Necessary for the timer to work
	gpio_set_mode(P_SW, PullUp);
}

/**
 * \brief Macro used to construct the name for the specialised timer interrupt for each DTMF tone.
 * 
 * This is used mainly to create the dispatch table.
 * 
 * @param F1 the base frequency (the higher frequency)
 * @param F2 the second frequency (the lower frequency)
 */
#define TIMER_CALLBACK_ISR_NAME(F1, F2) \
	timer_callback_isr_##F1##_##F2

/**
 * \brief Macro used to declare the specialised timer interrupt for each DTMF tone.
 *
 * @param F1 the base frequency (the higher frequency)
 * @param F2 the second frequency (the lower frequency)
 */
#define TIMER_CALLBACK_ISR(F1, F2) \
	static void TIMER_CALLBACK_ISR_NAME(F1, F2)(void)
	
TIMER_CALLBACK_ISR(1209, 697); // 1
TIMER_CALLBACK_ISR(1336, 697); // 2
TIMER_CALLBACK_ISR(1477, 697); // 3
TIMER_CALLBACK_ISR(1633, 697); // A

TIMER_CALLBACK_ISR(1209, 770); // 4
TIMER_CALLBACK_ISR(1336, 770); // 5
TIMER_CALLBACK_ISR(1477, 770); // 6
TIMER_CALLBACK_ISR(1633, 770); // B
	
TIMER_CALLBACK_ISR(1209, 852); // 7
TIMER_CALLBACK_ISR(1336, 852); // 8
TIMER_CALLBACK_ISR(1477, 852); // 9
TIMER_CALLBACK_ISR(1633, 852); // C
	
TIMER_CALLBACK_ISR(1209, 941); // *
TIMER_CALLBACK_ISR(1336, 941); // 0
TIMER_CALLBACK_ISR(1477, 941); // #
TIMER_CALLBACK_ISR(1633, 941); // D


/**
 * \brief Macro used to define the specialised timer interrupt for each DTMF tone.
 *
 * @param F1 the base frequency (the higher frequency)
 * @param F2 the second frequency (the lower frequency)
 */
#define TIMER_CALLBACK_ISR_DEF(F1, F2) \
	TIMER_CALLBACK_ISR(F1, F2) {         \
		timer_callback_isr(F1, F2);        \
	}
	
TIMER_CALLBACK_ISR_DEF(1209, 697); // 1
TIMER_CALLBACK_ISR_DEF(1336, 697); // 2
TIMER_CALLBACK_ISR_DEF(1477, 697); // 3
TIMER_CALLBACK_ISR_DEF(1633, 697); // A

TIMER_CALLBACK_ISR_DEF(1209, 770); // 4
TIMER_CALLBACK_ISR_DEF(1336, 770); // 5
TIMER_CALLBACK_ISR_DEF(1477, 770); // 6
TIMER_CALLBACK_ISR_DEF(1633, 770); // B
	
TIMER_CALLBACK_ISR_DEF(1209, 852); // 7
TIMER_CALLBACK_ISR_DEF(1336, 852); // 8
TIMER_CALLBACK_ISR_DEF(1477, 852); // 9
TIMER_CALLBACK_ISR_DEF(1633, 852); // C
	
TIMER_CALLBACK_ISR_DEF(1209, 941); // *
TIMER_CALLBACK_ISR_DEF(1336, 941); // 0
TIMER_CALLBACK_ISR_DEF(1477, 941); // #
TIMER_CALLBACK_ISR_DEF(1633, 941); // D
	
/**
*A dispatch table of function pointers to specialised DTMF tone interrupts. The array elements correspond to their symbols, and are organised in the same structure as the keypad.
*/

static void (*dispatch_table[N_COLS][N_ROWS])(void) = {
	{
		TIMER_CALLBACK_ISR_NAME(1209, 697), // 1
		TIMER_CALLBACK_ISR_NAME(1336, 697), // 2
		TIMER_CALLBACK_ISR_NAME(1477, 697), // 3
		TIMER_CALLBACK_ISR_NAME(1633, 697), // A
	},
	{
		TIMER_CALLBACK_ISR_NAME(1209, 770), // 4
		TIMER_CALLBACK_ISR_NAME(1336, 770), // 5
		TIMER_CALLBACK_ISR_NAME(1477, 770), // 6
		TIMER_CALLBACK_ISR_NAME(1633, 770), // B
	},
	{
		TIMER_CALLBACK_ISR_NAME(1209, 852), // 7
		TIMER_CALLBACK_ISR_NAME(1336, 852), // 8
		TIMER_CALLBACK_ISR_NAME(1477, 852), // 9
		TIMER_CALLBACK_ISR_NAME(1633, 852), // C
	},
	{
		TIMER_CALLBACK_ISR_NAME(1209, 941), // *
		TIMER_CALLBACK_ISR_NAME(1336, 941), // 0
		TIMER_CALLBACK_ISR_NAME(1477, 941), // #
		TIMER_CALLBACK_ISR_NAME(1633, 941), // D
	}
};

static unsigned base_freqs[N_COLS] = {1209, 1336, 1477, 1633};

__STATIC_INLINE void timer_callback_isr(unsigned base_freq, unsigned freq) {
	int sample = SIN_ADD(base_freq, freq, sample_index);
	sample_index += RATE_MULTIPLIER;
	dac_set(sample);
	
	if (sample_index >= (base_freq * NUM_STEPS * SYMBOL_LENGTH_MS) / 1000U) {
		timer_set_callback_delay(pop_and_dac_interrupt_enable, PERIOD_MS_TO_CYCLES(INTERSYMBOL_SPACING_MS));
	}
}

static void sinewave_init(void) {
	int n;
	for (n = 0; n < NUM_STEPS; n++) {
		sine_table[n] = (int)((DAC_MASK) * (1 + sin(n * 2 * PI / NUM_STEPS)) / 2);
	}
}

static void dac_interrupt_enable_unsafe(int col, int row)
{
    // reset sample index.
    sample_index = 0;
	
    timer_set_callback(dispatch_table[row][col], FREQ_HZ_TO_CYCLES(base_freqs[col] * NUM_STEPS / RATE_MULTIPLIER));
}

static void pop_and_dac_interrupt_enable(void)
{
    int symbol;
    if ((symbol = check_and_dequeue()) != INT_MIN)
    {
			dac_interrupt_enable_unsafe(COL(symbol), ROW(symbol));
    } else {
			dac_interrupt_disable();
		}
}

static bool dac_interrupt_enable(int col, int row)
{
    // get old value of the flag
    // NOTE: this has to be int not bool, so that the compiler doesn't get any funny ideas.
    int flag = 1;
		
		int r0, r1, r2, r3;
		__asm(
        "MOV r1, &dac_interrupt_flag\n\t"
        "MOV r2, #1\n"
        "L1:\n\t"
        "LDREX r0, [r1]\n\t"
        "STREX r3, r2, [r1]\n\t"
        "CMP r3, #0\n\t"
        "BNE L1\n\t"
        "DMB\n\t"
        "MOV flag, r0");

    if (!flag)
    {
        dac_interrupt_enable_unsafe(col, row);
    }
    return !flag; // return success
}

static void dac_interrupt_disable(void)
{
    dac_interrupt_flag = false;
    timer_disable();
}

/**
*First checks if the DAC is currently playing a tone. If it is, it plays a tone directly. If not it adds it to the queue.
*@param row symbol's row on keypad
*@param col symbol's column on keypad
*/

void tone_play_or_enqueue(int row, int col) {
		int symbol = SYMBOL(row, col);
		if (!dac_interrupt_enable(col, row)) {
			enqueue(symbol);
		}
		lcd_put_char(symbol_chars[symbol]);
}
