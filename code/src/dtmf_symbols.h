#ifndef DTMF_SYMBOLS_H
#define DTMF_SYMBOLS_H

#define SYMBOL_1 0x0
#define SYMBOL_2 0x1
#define SYMBOL_3 0x2
#define SYMBOL_A 0x3

#define SYMBOL_4 0x4
#define SYMBOL_5 0x5
#define SYMBOL_6 0x6
#define SYMBOL_B 0x7

#define SYMBOL_7 0x8
#define SYMBOL_8 0x9
#define SYMBOL_9 0xA
#define SYMBOL_C 0xB

#define SYMBOL_STAR 0xC
#define SYMBOL_0 0xD
#define SYMBOL_POUND 0xE
#define SYMBOL_D 0xF

#define COL(SYMBOL) ((SYMBOL)&0x3)
#define ROW(SYMBOL) ((SYMBOL) >> 2)

#define SYMBOL(ROW, COL) (((ROW) << 2) | (COL))

#define N_ROWS 4
#define N_COLS 4

static char symbol_chars[N_ROWS * N_COLS] = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D'};

#endif // DTMF_SYMBOLS_H
