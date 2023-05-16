#include "lcd.h"
#include "keypad.h"
#include "dtmf_symbols.h"
#include "menu.h"
#include "settings.h"
#include "tone.h"
#include "lpc_eeprom.h"
#include <stdio.h>
#include <string.h>

#define LCD_CHAR 16

Settings settings;

void boot_mode_init(void) {
	lcd_clear();
	lcd_print("1:KEYPD 2:QCKDL");
	lcd_set_cursor(0, 1);
	lcd_print("3:SETTINGS");
	keypad_set_read_callback(boot_menu_input);
}

void boot_menu_input(int row, int col){
	switch (SYMBOL(row, col)){
		case SYMBOL_1:
			lcd_clear();
			tone_init();
			keypad_set_read_callback(tone_play_or_enqueue);
			break;
				
		case SYMBOL_2:
			enter_quickdial_mode();
			break;
		
		case SYMBOL_3:
			settings_mode_init();
			break;
	}
				
}

void settings_mode_init(void){
	lcd_clear();
	lcd_print("1:ISS 2:SYMLEN");
	lcd_set_cursor(0, 1);
	lcd_print("3:QUALITY");
	keypad_set_read_callback(settings_menu_input);
}

void settings_menu_input(int row, int col) {
	switch (SYMBOL(row, col)) {
		case SYMBOL_1:
			set_inter_symbol_spacing_mode_init();
			break;
		
		case SYMBOL_2:
			set_symbol_length_mode_init();
			break;
		
		case SYMBOL_3:
			set_lut_logsize_mode_init();
			break;
	}
}

void set_inter_symbol_spacing_mode_init(void) {
	lcd_clear();
	display_menu_options();
	menu_prompt("ISS: %d", settings.inter_symbol_spacing);
	keypad_set_read_callback(set_inter_symbol_spacing_menu_input);
}

void set_inter_symbol_spacing_menu_input(int row, int col) {
	static int inter_symbol_spacing = 0;

	int symbol = SYMBOL(row, col);	
	switch (symbol) {
		case SYMBOL_0:
		case SYMBOL_1:
		case SYMBOL_2:
		case SYMBOL_3:
		case SYMBOL_4:
		case SYMBOL_5:
		case SYMBOL_6:
		case SYMBOL_7:
		case SYMBOL_8:
		case SYMBOL_9:
			lcd_put_char(symbol_chars[symbol]);
			keypad_input_to_number(row, col, &inter_symbol_spacing);
			break;
		
		case SYMBOL_POUND:
			if (inter_symbol_spacing >= MIN_INTER_SYMBOL_SPACING_MS &&
				  inter_symbol_spacing <= MAX_INTER_SYMBOL_SPACING_MS) {
				settings.inter_symbol_spacing = inter_symbol_spacing;
				store_settings();
			}
			inter_symbol_spacing = 0;
			
			boot_mode_init();
			break;
		
		case SYMBOL_STAR:
			inter_symbol_spacing = 0;
			clear_user_input();
			break;
	}
}

void set_symbol_length_mode_init(void) {
	lcd_clear();
	display_menu_options();
	menu_prompt("SYMLEN: %d", settings.symbol_length);
	keypad_set_read_callback(set_symbol_length_menu_input);
}
	
void set_symbol_length_menu_input(int row, int col) {
	static int symbol_length = 0;

	int symbol = SYMBOL(row, col);	
	switch (symbol) {
		case SYMBOL_0:
		case SYMBOL_1:
		case SYMBOL_2:
		case SYMBOL_3:
		case SYMBOL_4:
		case SYMBOL_5:
		case SYMBOL_6:
		case SYMBOL_7:
		case SYMBOL_8:
		case SYMBOL_9:
			lcd_put_char(symbol_chars[symbol]);
			keypad_input_to_number(row, col, &symbol_length);
			break;
		
		case SYMBOL_POUND:
			if (symbol_length >= MIN_SYMBOL_LENGTH_MS &&
				  symbol_length <= MAX_SYMBOL_LENGTH_MS) {
				settings.symbol_length = symbol_length;
				store_settings();
			}
			symbol_length = 0;
			
			boot_mode_init();
			break;
		
		case SYMBOL_STAR:
			symbol_length = 0;
			clear_user_input();
			break;
	}
}

void set_lut_logsize_mode_init(void) {
	lcd_clear();
	display_menu_options();
	menu_prompt("QUALITY: %d", settings.symbol_length);
	keypad_set_read_callback(set_symbol_length_menu_input);
}
	
void set_lut_logsize_menu_input(int row, int col) {
	static int lut_logsize = 0;

	int symbol = SYMBOL(row, col);	
	switch (symbol) {
		case SYMBOL_0:
		case SYMBOL_1:
		case SYMBOL_2:
		case SYMBOL_3:
		case SYMBOL_4:
		case SYMBOL_5:
		case SYMBOL_6:
		case SYMBOL_7:
		case SYMBOL_8:
		case SYMBOL_9:
			lcd_put_char(symbol_chars[symbol]);
			keypad_input_to_number(row, col, &lut_logsize);
			break;
		
		case SYMBOL_POUND:
			if (lut_logsize >= MIN_LUT_LOGSIZE &&
				  lut_logsize <= MAX_LUT_LOGSIZE) {
				settings.lut_logsize = lut_logsize;
				store_settings();
			}
			lut_logsize = 0;
			
			boot_mode_init();
			break;
		
		case SYMBOL_STAR:
			lut_logsize = 0;
			clear_user_input();
			break;
	}
}

void check_settings(Settings *settings) {
	if (settings->checksum != SETTINGS_CHECKSUM(*settings)) {
		settings->inter_symbol_spacing = DEFAULT_INTER_SYMBOL_SPACING_MS;
		settings->lut_logsize = DEFAULT_LUT_LOGSIZE;
		settings->symbol_length = DEFAULT_SYMBOL_LENGTH_MS;
	}
	
	if (settings->inter_symbol_spacing < MIN_INTER_SYMBOL_SPACING_MS || 
		  settings->inter_symbol_spacing > MAX_INTER_SYMBOL_SPACING_MS) {
		settings->inter_symbol_spacing = DEFAULT_INTER_SYMBOL_SPACING_MS;
	}
	
	if (settings->lut_logsize < MIN_LUT_LOGSIZE || 
	    settings->lut_logsize > MAX_LUT_LOGSIZE) {
		settings->lut_logsize = DEFAULT_LUT_LOGSIZE;
	}
			
	if (settings->symbol_length < MIN_SYMBOL_LENGTH_MS ||
		  settings->symbol_length > MIN_SYMBOL_LENGTH_MS) {
		settings->symbol_length = DEFAULT_SYMBOL_LENGTH_MS;
	}
}

void load_settings() {
	Settings loaded;
	EEPROM_Read(SETTINGS_PAGE, SETTINGS_OFFSET, (void*)&loaded, MODE_16_BIT, sizeof(Settings) >> 1);
	
	// verify validity of settings
	check_settings(&loaded);
	
	settings = loaded;
}

void store_settings() {
	settings.checksum = SETTINGS_CHECKSUM(settings);
	EEPROM_Write(SETTINGS_PAGE, SETTINGS_OFFSET, (void*)&settings, MODE_16_BIT, sizeof(Settings) >> 1);
}

