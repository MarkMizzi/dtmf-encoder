#include "dtmf_symbols.h"
#include "lcd.h"
#include "menu.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/** String used to display menu options to the user.
 * 
 * Used in menus that process keypad input from the user.
 */
#define MENU_OPTIONS "*:CLR   #:OK"

/** Number of characters in a single line of the LCD screen.
 */
#define LCD_CHARS 16

/** Buffer used to temporarily hold content for a single line of the LCD screen.
 *
 * This is useful when, for example, we need to use sprintf().
 */
char lcd_string[LCD_CHARS] = {0};

/** Length of the currently displayed prompt. 
 *
 * This is set by menu_prompt(), and is used by clear_user_input() to 
 * return the cursor to its original position before user input.
 */
int prompt_length = 0;

void display_menu_options() {
	lcd_set_cursor(0, 1);
	lcd_print(MENU_OPTIONS);
}

void menu_prompt(const char *prompt, ...) {
  va_list args;
	
	memset(lcd_string, 0, LCD_CHARS);
	sprintf(lcd_string, prompt, args);
	
	lcd_set_cursor(0, 0);
	lcd_print(lcd_string);
	
	prompt_length = strlen(lcd_string);
	
	lcd_print(" ");
	lcd_set_cursor_visibile(1);
}

void clear_user_input() {
	int i = prompt_length;
	
	lcd_set_cursor(prompt_length, 0);
	for (; i < LCD_CHARS; ++i) {
		lcd_put_char(' ');
	}
	lcd_set_cursor(prompt_length + 1, 0);
}

void keypad_input_to_number(int row, int col, int *curr) {
	int digit;
	switch (SYMBOL(row, col)) {
		case SYMBOL_0:
			digit = 0;
			break;
		
		case SYMBOL_1:
			digit = 1;
			break;
		
		case SYMBOL_2:
			digit = 2;
			break;
		
		case SYMBOL_3:
			digit = 3;
			break;
		
		case SYMBOL_4:
			digit = 4;
			break;
		
		case SYMBOL_5:
			digit = 5;
			break;
		
		case SYMBOL_6:
			digit = 6;
			break;
		
		case SYMBOL_7:
			digit = 7;
			break;
		
		case SYMBOL_8:
			digit = 8;
			break;
		
		case SYMBOL_9:
			digit = 9;
			break;
	}
	
	*curr = (*curr) * 10 + digit;
}
