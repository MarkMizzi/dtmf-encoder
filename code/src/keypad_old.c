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
	gpio_set_mode(P_COL_0, PullUp);
	gpio_set_mode(P_COL_1, PullUp);
	gpio_set_mode(P_COL_2, PullUp);
	gpio_set_mode(P_COL_3, PullUp);
	//configuring the row pins
	gpio_set_mode(P_ROW_0, Output);
	gpio_set_mode(P_ROW_1, Output);
	gpio_set_mode(P_ROW_2, Output);
	gpio_set_mode(P_ROW_3, Output);
	
	//setting all the cols as active lows
	set_rows(0,0,0,0);
}

//setting 
void set_rows(int row_0, int row_1, int row_2, int row_3)
{
	gpio_set(P_ROW_0, row_0);
	gpio_set(P_ROW_1, row_1);
	gpio_set(P_ROW_2, row_2);
	gpio_set(P_ROW_3, row_3);
}

/*
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
*/

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
	int col_pos, row_pos;
	//intializing the keypad
	keypad_init();
	
	while(true)
	{
		for (col_pos = 0; col_pos < COLS; col_pos++)
		{
			if (gpio_get(GET_COL_PIN(col_pos)) == 0)
				{
				for (row_pos = 0; row_pos < ROWS; row_pos++) 
				{
					gpio_set(GET_ROW_PIN(row_pos), 1);
					if (gpio_get(GET_COL_PIN(col_pos)) == 1)
					{
						start_or_enqueue_modified(col_pos, row_pos);
						lcd_put_char(key_chars[row_pos][col_pos]);
					}
				}
			}	
		}
		set_rows(0, 0, 0, 0);
		delay_ms(200);
	}
}