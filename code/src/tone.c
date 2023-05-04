#include <platform.h>
#include <math.h>
#include <dac.h>
#include <adc.h>
#include <timer.h>
#include <gpio.h>
#include "tone.h"
#include "delay.h"
#include "queue.h"
#include "dtmf_symbols.h"

#define SYMBOL_LENGTH_MS 2000

#define NUM_STEPS 64
#define QUEUE_SIZE 16

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

#define PI 3.1415927

int sine_table[NUM_STEPS];
Queue play_queue;

void timer_callback_isr(unsigned base_freq, unsigned freq);
void sinewave_init(void);

void tone_init(void) {
	queue_init(&play_queue, QUEUE_SIZE);
	dac_init();
	sinewave_init();
	
	gpio_set_mode(P_SW, PullUp);
}

static unsigned sample_index = 0;

#define TIMER_CALLBACK_ISR_NAME(F1, F2) \
	timer_callback_isr_##F1##_##F2

#define TIMER_CALLBACK_ISR(F1, F2) \
	void TIMER_CALLBACK_ISR_NAME(F1, F2)(void)

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
	
void (*dispatch_table[N_COLS][N_ROWS])(void) = {
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
	
void tone_play_with_interrupt(unsigned col, unsigned row) {
	timer_set_callback(dispatch_table[row][col], base_freqs[col]);
}
	
void timer_callback_isr(unsigned base_freq, unsigned freq) {
	int sample = SIN_ADD(base_freq, freq, sample_index);
	sample_index++;
	dac_set(sample);
	
	if (sample_index >= (base_freq * NUM_STEPS * SYMBOL_LENGTH_MS) / 1000UL) {
		sample_index = 0;
		timer_disable();
	}
}

void sinewave_init(void) {
	int n;
	for (n = 0; n < NUM_STEPS; n++) {
		sine_table[n] = (int)((DAC_MASK) * (1 + sin(n * 2 * PI / NUM_STEPS)) / 2);
	}
}

// *******************************ARM University Program Copyright © ARM Ltd 2014*************************************   
