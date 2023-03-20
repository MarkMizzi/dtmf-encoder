#include "dtmf_symbols.h"
#include "queue.h"
#include "dac_interrupt.h"
#include "drivers/timer.h"
#include "drivers/dac.h"

#include "LPC407x_8x_177x_8x.h"

#include <limits.h>
#include <stdbool.h>

// NOTE: when we add custom settings, these should be the things to change.
#define LOG_2_N 7
#define LOG_2_K 2

#define K (1 << LOG_2_K)
#define N (1 << LOG_2_N)

const static unsigned sin_lut[N] = {
    511, 536, 561, 586, 611, 635, 659, 683, 707, 730, 752,
    774, 795, 816, 835, 855, 873, 890, 906, 922, 936, 950,
    962, 973, 984, 993, 1000, 1007, 1013, 1017, 1020, 1022,
    1023, 1022, 1020, 1017, 1013, 1007, 1000, 993, 984, 973,
    962, 950, 936, 922, 906, 890, 873, 855, 835, 816, 795,
    774, 752, 730, 707, 683, 659, 635, 611, 586, 561, 536,
    511, 486, 461, 436, 411, 387, 363, 339, 315, 292, 270,
    248, 227, 206, 187, 167, 149, 132, 116, 100, 86, 72,
    60, 49, 38, 29, 22, 15, 9, 5, 2, 0, 0, 0, 2, 5,
    9, 15, 22, 29, 38, 49, 60, 72, 86, 100, 116, 132,
    149, 167, 187, 206, 227, 248, 270, 292, 315, 339, 363,
    387, 411, 436, 461, 486};

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
    sin_lut[((i)*K) & 0x7f] + SIN((f1), (f2), (i))

void dac_interrupt_callback(unsigned f1, unsigned f2);
bool pop_and_dac_interrupt_enable(void);
void dac_interrupt_enable_unsafe(int col, int row);
void dac_interrupt_disable(void);

void dac_interrupt_callback(unsigned f1, unsigned f2)
{
    // generate tone (no DAC to drive here).
    unsigned sample = SIN_ADD(f1, f2, sample_index);
    dac_set((int)sample);

    // TODO: Drive DAC

    sample_index++;

    // TODO: if sample_index > f1 * K * symbol_length, initiate callback termination.
    // This needs to disable the current callback and check if there is another pending symbol.
    // IMPORTANT: Put this code in its own routine, otherwise it will be duplicated 16x,
    //     a waste for code not executed frequently

    if (sample_index >= (f1 * SYMBOL_LENGTH) << (LOG_2_N - LOG_2_K))
    {
        // start off next tone
        pop_and_dac_interrupt_enable();
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

void dac_interrupt_enable_unsafe(int col, int row)
{
    // reset sample index.
    sample_index = 0;
    void (*handler)(void) = dispatch_table[row][col];
    unsigned timer_freq = col_freqs[col] << (LOG_2_N - LOG_2_K);

    timer_init(SystemCoreClock / timer_freq);
    timer_set_callback(handler);
    timer_enable();
}

bool pop_and_dac_interrupt_enable(void)
{
    // TODO: This is NOT final, it needs to start an extra handler which adds a delay
    int symbol;
    if ((symbol = check_and_dequeue()) != INT_MIN)
    {
        dac_interrupt_enable_unsafe(COL(symbol), ROW(symbol));
        return true;
    }
    dac_interrupt_disable();
    return false;
}

bool dac_interrupt_enable(int col, int row)
{
    // get old value of the flag
    // NOTE: this has to be int not bool, so that the compiler doesn't get any funny ideas.
    int flag;
    __asm(
        "MOV r1, %[dac_interrupt_flag]\n\t"
        "MOV r2, #1\n"
        "L1:\n\t"
        "LDREX r0, [r1]\n\t"
        "STREX r3, r2, [r1]\n\t"
        "CMP r3, #0\n\t"
        "BNE L1\n\t"
        "DMB\n\t"
        "MOV %[flag], r0"
        : [flag] "=r"(flag)
        : [dac_interrupt_flag] "r"(&dac_interrupt_flag)
        : "r0", "r1", "r2", "r3");

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
