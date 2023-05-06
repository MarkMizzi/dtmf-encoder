#include "keypad.h"
#include "delay.h"
#include <gpio.h>

void keypad_init(void) {
	
	gpio_set_mode(P_COL_0, Output);
	gpio_set_mode(P_COL_1, Output);
	gpio_set_mode(P_COL_2, Output);
	gpio_set_mode(P_COL_3, Output);
	
	gpio_set(P_COL_0, 1);
	gpio_set(P_COL_1, 1);
	gpio_set(P_COL_2, 1);
	gpio_set(P_COL_3, 1);
	
	gpio_set_mode(P_ROW_0, PullUp);
	gpio_set_mode(P_ROW_1, PullUp);
	gpio_set_mode(P_ROW_2, PullUp);
	gpio_set_mode(P_ROW_3, PullUp);
}

void read_keypad(void (*callback)(int, int)) {
	int col, row;
	
	for (col = 0; col < KEYPAD_COLS; col++) {
		gpio_set(colno_to_pin[col], 0);
		delay_ms(200);
		
		for (row = 0; row < KEYPAD_ROWS; row++) {
			if (gpio_get(rowno_to_pin[row]) == 0) {
				callback(row, col);
			}
		}
		
		gpio_set(colno_to_pin[col], 1);
		delay_us(100);
	}
}