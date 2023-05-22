#ifndef DTMF_SYMBOLS_H
#define DTMF_SYMBOLS_H

/** \brief Symbol representing a keypad input of 1.
 */
#define SYMBOL_1 0x0
/** \brief Symbol representing a keypad input of 2.
 */
#define SYMBOL_2 0x1
/** \brief Symbol representing a keypad input of 3.
 */
#define SYMBOL_3 0x2
/** \brief Symbol representing a keypad input of A.
 */
#define SYMBOL_A 0x3

/** \brief Symbol representing a keypad input of 4.
 */
#define SYMBOL_4 0x4
/** \brief Symbol representing a keypad input of 5.
 */
#define SYMBOL_5 0x5
/** \brief Symbol representing a keypad input of 6.
 */
#define SYMBOL_6 0x6
/** \brief Symbol representing a keypad input of B.
 */
#define SYMBOL_B 0x7

/** \brief Symbol representing a keypad input of 7.
 */
#define SYMBOL_7 0x8
/** \brief Symbol representing a keypad input of 8.
 */
#define SYMBOL_8 0x9
/** \brief Symbol representing a keypad input of 9.
 */
#define SYMBOL_9 0xA
/** \brief Symbol representing a keypad input of B.
 */
#define SYMBOL_C 0xB

/** \brief Symbol representing a keypad input of `*`.
 */
#define SYMBOL_STAR 0xC
/** \brief Symbol representing a keypad input of 0.
 */
#define SYMBOL_0 0xD
/** \brief Symbol representing a keypad input of `#`.
 */
#define SYMBOL_POUND 0xE
/** \brief Symbol representing a keypad input of D.
 */
#define SYMBOL_D 0xF

/** \brief Convert a symbol representing some key on the keypad to its column position.
 */
#define COL(SYMBOL) ((SYMBOL)&0x3)
/** \brief Convert a symbol representing some key on the keypad to its row position.
 */
#define ROW(SYMBOL) ((SYMBOL) >> 2)

/** \brief Construct a symbol from its row and column position.
 */
#define SYMBOL(ROW, COL) (((ROW) << 2) | (COL))

/** \brief Constant containing number of rows in the DTMF keypad.
 */
#define N_ROWS 4
/** \brief Constant containing number of columns in the DTMF keypad.
 */
#define N_COLS 4

/** \brief Array containing characters of the DTMF symbols in order of the constant `SYMBOL_` values.
 */
static char symbol_chars[N_ROWS * N_COLS] = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D'};

#endif // DTMF_SYMBOLS_H
