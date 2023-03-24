#include "dtmf_symbols.h"
#include "dac_interrupt.h"
#include "timer.h"
#include "queue.h"

#include <dac.h>
#include <platform.h>
#include <LPC407x_8x_177x_8x.h>
#include <limits.h>
#include <stdbool.h>

// NOTE: when we add custom settings, these should be the things to change.
#define LOG_2_N 7
#define LOG_2_K 2

#define K (1 << LOG_2_K)
#define N (1 << LOG_2_N)

const static unsigned sin_lut[N] = {
  128,134,140,146,152,158,165,170,
	176,182,188,193,198,203,208,213,
	218,222,226,230,234,237,240,243,
	245,248,250,251,253,254,254,255,
	255,255,254,254,253,251,250,248,
	245,243,240,237,234,230,226,222,
	218,213,208,203,198,193,188,182,
	176,170,165,158,152,146,140,134,
	128,121,115,109,103, 97, 90, 85,   
	 79, 73, 67, 62, 57, 52, 47, 42,        
	 37, 33, 29, 25, 21, 18, 15, 12,        
	 10,  7,  5,  4,  2,  1,  1,  0,
	  0,  0,  1,  1,  2,  4,  5,  7,
	 10, 12, 15, 18, 21, 25, 29, 33,
	 37, 42, 47, 52, 57, 62, 67, 73,
	 79, 85, 90, 97,103,109,115,121,
};
		
const static unsigned col_freqs[] = {1209, 1336, 1477, 1633};
const static unsigned row_freqs[] = {697, 770, 852, 941};

// is a tone currently being generated?
static int dac_interrupt_flag = 0;
static unsigned sample_index = 0;

#define SIN(BASEFREQ, FREQ, IDX)                            \
    ((sin_lut[((FREQ) * (IDX)*K / (BASEFREQ)) & 0x7f] +     \
      sin_lut[-(-(FREQ) * (IDX)*K / (BASEFREQ)) & 0x7f]) >> \
     1)

#define SIN_ADD(f1, f2, i) \
    (sin_lut[((i)*K) & 0x7f] + SIN((f1), (f2), (i))) >> 1

static void dac_interrupt_callback(unsigned f1, unsigned f2);
static void pop_and_dac_interrupt_enable(void);
static void start_pop_and_dac_interrupt_enable(void);
static void dac_interrupt_enable_unsafe(int col, int row);
static void dac_interrupt_disable(void);

static void dac_interrupt_callback(unsigned f1, unsigned f2)
{
    unsigned sample = SIN_ADD(f1, f2, sample_index);
    dac_set((int)sample);
    sample_index++;

		if (sample_index >= (f1 * SYMBOL_LENGTH) << (LOG_2_N - LOG_2_K))
    {
        // start off next tone
        start_pop_and_dac_interrupt_enable();
    }
}

#define DAC_INTERRUPT_CALLBACK_NAME(f1, f2) \
    dac_interrupt_callback_##f1##_##f2

#define DEF_DAC_INTERRUPT_CALLBACK(f1, f2)                \
    static void DAC_INTERRUPT_CALLBACK_NAME(f1, f2)(void) \
    {                                                     \
        dac_interrupt_callback(f1, f2);                   \
    }

DEF_DAC_INTERRUPT_CALLBACK(1209, 697) // 1
DEF_DAC_INTERRUPT_CALLBACK(1336, 697) // 2
DEF_DAC_INTERRUPT_CALLBACK(1477, 697) // 3
DEF_DAC_INTERRUPT_CALLBACK(1633, 697) // A

DEF_DAC_INTERRUPT_CALLBACK(1209, 770) // 4
DEF_DAC_INTERRUPT_CALLBACK(1336, 770) // 5
DEF_DAC_INTERRUPT_CALLBACK(1477, 770) // 6
DEF_DAC_INTERRUPT_CALLBACK(1633, 770) // B

DEF_DAC_INTERRUPT_CALLBACK(1209, 852) // 7
DEF_DAC_INTERRUPT_CALLBACK(1336, 852) // 8
DEF_DAC_INTERRUPT_CALLBACK(1477, 852) // 9
DEF_DAC_INTERRUPT_CALLBACK(1633, 852) // C

DEF_DAC_INTERRUPT_CALLBACK(1209, 941) // *
DEF_DAC_INTERRUPT_CALLBACK(1336, 941) // 0
DEF_DAC_INTERRUPT_CALLBACK(1477, 941) // #
DEF_DAC_INTERRUPT_CALLBACK(1633, 941) // D

static void (*dispatch_table[4][4])(void) = {
    {
        DAC_INTERRUPT_CALLBACK_NAME(1209, 697), // 1
        DAC_INTERRUPT_CALLBACK_NAME(1336, 697), // 2
        DAC_INTERRUPT_CALLBACK_NAME(1477, 697), // 3
        DAC_INTERRUPT_CALLBACK_NAME(1633, 697), // A
    },
    {
        DAC_INTERRUPT_CALLBACK_NAME(1209, 770), // 4
        DAC_INTERRUPT_CALLBACK_NAME(1336, 770), // 5
        DAC_INTERRUPT_CALLBACK_NAME(1477, 770), // 6
        DAC_INTERRUPT_CALLBACK_NAME(1633, 770), // B
    },
    {
        DAC_INTERRUPT_CALLBACK_NAME(1209, 852), // 7
        DAC_INTERRUPT_CALLBACK_NAME(1336, 852), // 8
        DAC_INTERRUPT_CALLBACK_NAME(1477, 852), // 9
        DAC_INTERRUPT_CALLBACK_NAME(1633, 852), // C
    },
    {
        DAC_INTERRUPT_CALLBACK_NAME(1209, 941), // *
        DAC_INTERRUPT_CALLBACK_NAME(1336, 941), // 0
        DAC_INTERRUPT_CALLBACK_NAME(1477, 941), // #
        DAC_INTERRUPT_CALLBACK_NAME(1633, 941), // D
    },
};

static void dac_interrupt_enable_unsafe(int col, int row)
{
    unsigned timer_freq = col_freqs[col] << (LOG_2_N - LOG_2_K);
	
    // reset sample index.
    sample_index = 0;
	
    timer_enable(dispatch_table[row][col], timer_freq);
}

static void pop_and_dac_interrupt_enable(void)
{
    // TODO: This is NOT final, it needs to start an extra handler which adds a delay
    int symbol;
    if ((symbol = check_and_dequeue()) != INT_MIN)
    {
        dac_interrupt_enable_unsafe(COL(symbol), ROW(symbol));
    }
    dac_interrupt_disable();
}

static void start_pop_and_dac_interrupt_enable(void) {
	timer_enable(pop_and_dac_interrupt_enable, 1/INTERSYMBOL_SPACING);
}

bool dac_interrupt_enable(int col, int row)
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

void dac_interrupt_disable(void)
{
    dac_interrupt_flag = false;
    timer_disable();
}
