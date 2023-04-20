#pragma once

#include <gpio.h>
#include <platform.h>
#include "keypad.h"

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

#define ROWS 4
#define COLS 4

void keypad_init();
																			
void set_rows(int row_0, int row_1, int row_2, int row_3);
																			
void set_cols_all(int all_rows);

int read_columns();

void read_keypad();
																			
void start_or_enqueue(int symbol);
																			
void start_or_enqueue_modified(int col, int row);