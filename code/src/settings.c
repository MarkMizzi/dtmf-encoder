#include "lcd.h"
#include "keypad.h"
#include "dtmf_symbols.h"
#include "menu.h"
#include "settings.h"
#include "tone.h"
#include "lpc_eeprom.h"
#include <stdio.h>
#include <string.h>

#define CHECKSUM(SETTINGS) \
	((SETTINGS).inter_symbol_spacing ^ (SETTINGS).symbol_length ^ (SETTINGS).sampling_rate_multiplier)

#define LCD_CHAR 16

Settings settings;

char lcd_string[LCD_CHAR] = {0};

void enter_boot_mode(void) {
	load_settings();
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
			keypad_set_read_callback(tone_play_or_enqueue);
			break;
				
		case SYMBOL_2:
			enter_quickdial_mode();
			break;
		
		case SYMBOL_3:
			enter_settings_mode();
			break;
	}
				
}

void enter_settings_mode(void){
	lcd_clear();
	lcd_print("1:ISS 2:SYMLEN");
	lcd_set_cursor(0, 1);
	lcd_print("3:SAMP RATE MUL");
	keypad_set_read_callback(settings_menu_input);
}

void load_settings() {
	Settings loaded;
	EEPROM_Read(SETTINGS_PAGE, SETTINGS_OFFSET, (void*)&loaded, MODE_16_BIT, sizeof(Settings) >> 1);
	
	if (loaded.checksum != CHECKSUM(loaded)) {
		loaded.inter_symbol_spacing = DEFAULT_INTER_SYMBOL_SPACING_MS;
		loaded.sampling_rate_multiplier = DEFAULT_SAMPLING_RATE_MULTIPLIER;
		loaded.symbol_length = DEFAULT_SYMBOL_LENGTH_MS;
	}
	
	settings = loaded;
}

void store_settings() {
	settings.checksum = CHECKSUM(settings);
	EEPROM_Write(SETTINGS_PAGE, SETTINGS_OFFSET, (void*)&settings, MODE_16_BIT, sizeof(Settings) >> 1);
}
