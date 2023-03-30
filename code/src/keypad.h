#pragma once

#include <gpio.h>
#include <platform.h>
#include "keypad.h"

//defining macros for the pins connected to the columns
#define P_COL_0 P3_0
#define P_COL_1	P3_1
#define P_COL_2 P3_2
#define P_COL_3 P3_3

//defining macros for the pins connected to the rows
#define P_ROW_0 P4_0
#define P_ROW_1 P4_1
#define P_ROW_2 P4_2
#define P_ROW_3 P4_3

#define ROWS 4
#define COLS 4

void keypad_init();
																			
void set_cols(int row_0, int row_1, int row_2, int row_3);
																			
void set_cols_all(int all_rows);

int read_columns();

void read_keypad();
																			
void start_or_enqueue(int symbol);
																			
void start_or_enqueue_modified(int col, int row);