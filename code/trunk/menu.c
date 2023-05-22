#include "lcd.h"
#include "menu.h"
#include <stdarg.h>
#include <stdio.h>

#define MENU_OPTIONS "#: CLR   *: OK"

#define LCD_CHARS 16

char lcd_string[LCD_CHARS];

void display_menu_options() {
	lcd_set_cursor(0, 1);
	lcd_print(MENU_OPTIONS);
}

void menu_prompt(const char *prompt, ...) {
  va_list args;
	sprintf(lcd_string, prompt, args);
	lcd_set_cursor(0, 0);
	lcd_print(lcd_string);
	lcd_print(" ");
	lcd_set_cursor_visibile(1);
}
