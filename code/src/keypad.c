#include <dac.h>
#include "keypad.h"
#include "dtmf_symbols.h"
#include "dac_interrupt.h"
#include "queue.h"
#include "lcd.h"
#include "delay.h"

#define COL_VALUE(COLUMNS, COL_POS) \
		COLUMNS >> (3-COL_POS)

const char key_chars[ROWS][COLS] =
{
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}
};

void start_or_enqueue_modified(int col, int row)
{
	int symbol = (row << 2) | col;
	if (!dac_interrupt_enable(col, row)) {
		enqueue(symbol);
	}
}

void keypad_init() 
{
	//configuring the column pins
	gpio_set_mode(P_COL_0, Output);
	gpio_set_mode(P_COL_1, Output);
	gpio_set_mode(P_COL_2, Output);
	gpio_set_mode(P_COL_3, Output);
	//configuring the row pins
	gpio_set_mode(P_ROW_0, PullDown);
	gpio_set_mode(P_ROW_1, PullDown);
	gpio_set_mode(P_ROW_2, PullDown);
	gpio_set_mode(P_ROW_3, PullDown);
	
	//setting all the rows as active lows
	set_cols(0,0,0,0);
}

//setting 
void set_cols(int col_0, int col_1, int col_2, int col_3)
{
	gpio_set(P_COL_0, col_0);
	gpio_set(P_COL_1, col_1);
	gpio_set(P_COL_2, col_2);
	gpio_set(P_COL_3, col_3);
}

//setting all the rows using 1 hexadecimal value
void set_cols_all(int hex_value)
{
	//extracting the bits
	int col_0, col_1, col_2, col_3;
	col_0 = (hex_value >> 3) & 0x1;
	col_1 = (hex_value >> 2) & 0x1;
	col_2 = (hex_value >> 1) & 0x1;
	col_3 &= 0x1;
	
	//setting the columns
	gpio_set(P_COL_0, col_0);
	gpio_set(P_COL_1, col_1);
	gpio_set(P_COL_2, col_2);
	gpio_set(P_COL_3, col_3);
}

int read_columns()
{
	return gpio_get_range(P_COL_0, 4);
}	

//looking for a way to optimize these functions
Pin GET_COL_PIN(int col_pos)
{
	if (col_pos == 0)
		return P_COL_0;
	else if (col_pos == 1)
		return P_COL_1;
	else if (col_pos == 2)
		return P_COL_2;
	else
		return P_COL_3;
}

Pin GET_ROW_PIN(int row_pos)
{
	if (row_pos == 0)
		return P_ROW_0;
	else if (row_pos == 1)
		return P_ROW_1;
	else if (row_pos == 2)
		return P_ROW_2;
	else
		return P_ROW_3;
}

void read_keypad()
{
	int row_values, col_values;
	int row;
	int col_pos;
	row_values = col_values = 0x0;
	//intializing the keypad
	keypad_init();
	while(true)
	{
		//loop unrolling - setting each column high and then reading the rows
		for (col_pos = 0; col_pos < COLS; col_pos++)
		{
			//setting the column
			gpio_set(GET_COL_PIN(col_pos), 1);
			//unrolled the loop
			//1st row
			if (gpio_get(P_ROW_0) == 1)
			{
				//generating the tone and printing to LCD
				start_or_enqueue_modified(col_pos, 0);
				lcd_put_char(key_chars[0][col_pos]);
				break;
			}
			//2nd row
			if (gpio_get(P_ROW_1) == 1)
			{
				start_or_enqueue_modified(col_pos, 1);
				lcd_put_char(key_chars[1][col_pos]);
				break;
			}
			//3rd row
			if (gpio_get(P_ROW_2) == 1)
			{
				start_or_enqueue_modified(col_pos, 2);
				lcd_put_char(key_chars[2][col_pos]);
				break;
			}
			//4th row
			if (gpio_get(P_ROW_3) == 1)
			{
				start_or_enqueue_modified(col_pos, 3);
				lcd_put_char(key_chars[3][col_pos]);
				break;
			}
			//resetting the column value to 0
			gpio_set(GET_COL_PIN(col_pos), 0);
		}
		//adding the delay
		delay_ms(200);
	}
}