#include "lcd.h"
#include "keypad.h"
#include "dtmf_symbols.h"
#include "settings.h"
#include "tone.h"
#include "lpc_eeprom.h"
#include <stdio.h>


#define LCD_CHAR 33

int BOOT_MODE;
int rate_multiplier;
int symbol_length_ms;
int intersymbol_spacing_ms;
char string[LCD_CHAR] = {0};

void boot_init(void){
	lcd_clear();
	lcd_print("KEYPD:1  QCKDL:2");
	lcd_print("STTNG:3");
	keypad_set_read_callback(boot);
	BOOT_MODE = 0;
}

void settings_init(void){
	Settings Current;
	EEPROM_Read(0, 0, &Current, MODE_16_BIT, sizeof(Settings)>>1);
	rate_multiplier = Current.sampling_rate_multiplier;
	intersymbol_spacing_ms = Current.inter_symbol_spacing;
	symbol_length_ms = Current.symbol_length;
	lcd_clear();
	sprintf(string, "ISS: %d", Current.inter_symbol_spacing);
	lcd_print(string);
	
}

void boot(int row, int col){
	switch (BOOT_MODE){
		case 0:
			switch (SYMBOL(row, col)){
				case SYMBOL_1:
					lcd_clear();
					keypad_set_read_callback(tone_play_or_enqueue);
					break;
				/*
				case SYMBOL_2:
					lcd_clear();
					quick_dial_init();
					keypad_set_read_callback(quick_dial);
					break;
				case SYMBOL_3:
					lcd_clear();
					settings_init();
					keypad_set_read_callback(settings_mode);
					break;
				*/
				break;
				}
				
	}
}
