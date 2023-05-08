#pragma once
#include <platform.h>

//defining macros for the pins connected to the columns
//32 - 29 columns
#define P_COL_0 P1_3
#define P_COL_1	P1_5
#define P_COL_2 P1_6
#define P_COL_3 P1_7

//defining macros for the pins connected to the rows
//28 - 25 rows
#define P_ROW_0 P0_21
#define P_ROW_1 P0_0
#define P_ROW_2 P0_1
#define P_ROW_3 P0_9

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

static Pin colno_to_pin[KEYPAD_COLS] = {P_COL_0, P_COL_1, P_COL_2, P_COL_3};
static Pin rowno_to_pin[KEYPAD_ROWS] = {P_ROW_0, P_ROW_1, P_ROW_2, P_ROW_3};

void keypad_init(void);
void read_keypad(void (*callback)(int, int));
