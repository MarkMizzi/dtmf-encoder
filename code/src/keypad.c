#include "keypad.h"
#include "delay.h"
#include <platform.h>
#include <gpio.h>
#include <stddef.h>

void (*read_keypad_callback)(int, int) = NULL;

void keypad_set_read_callback(void (*callback)(int, int)) {
	read_keypad_callback = callback;
}

static void read_keypad(int);

void keypad_init(void) {
	
	gpio_set_mode(P_COL_0, Output);
	gpio_set_mode(P_COL_1, Output);
	gpio_set_mode(P_COL_2, Output);
	gpio_set_mode(P_COL_3, Output);
	
	gpio_set(P_COL_0, 0);
	gpio_set(P_COL_1, 0);
	gpio_set(P_COL_2, 0);
	gpio_set(P_COL_3, 0);
	delay_us(100);
	
	gpio_set_mode(P_ROW_0, PullUp);
	gpio_set_mode(P_ROW_1, PullUp);
	gpio_set_mode(P_ROW_2, PullUp);
	gpio_set_mode(P_ROW_3, PullUp);
	
	gpio_set_mode(P_INTERRUPT, PullUp);
	gpio_set_trigger(P_INTERRUPT, Falling);
	
	// sets callback for any pin in this port.
	gpio_set_callback(P_INTERRUPT, read_keypad);
}

void read_keypad(int sources) {
	int col, row;
	
	if (!(sources & (1 << GET_PIN_INDEX(P_INTERRUPT)))) {
		// source of interrupt was not one of the row pins
		return;
	}
	
	// temporarily disable interrupts on pins.
	gpio_set_trigger(P_INTERRUPT, None);
	
	gpio_set(P_COL_0, 1);
	gpio_set(P_COL_1, 1);
	gpio_set(P_COL_2, 1);
	gpio_set(P_COL_3, 1);
	delay_us(100);
	
	for (col = 0; col < KEYPAD_COLS; col++) {
		gpio_set(colno_to_pin[col], 0);
		delay_us(100);
		
		for (row = 0; row < KEYPAD_ROWS; row++) {
			if (gpio_get(rowno_to_pin[row]) == 0) {
				if (read_keypad_callback != NULL) {
					read_keypad_callback(row, col);
				}
			}
		}
		
		gpio_set(colno_to_pin[col], 1);
		delay_us(100);
	}
	
	gpio_set(P_COL_0, 0);
	gpio_set(P_COL_1, 0);
	gpio_set(P_COL_2, 0);
	gpio_set(P_COL_3, 0);
	delay_us(100);
	
	// re-enable interrupts on pins.
	gpio_set_trigger(P_INTERRUPT, Falling);
	delay_ms(80);
}
