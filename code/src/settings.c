#include "lcd.h"
#include "keypad.h"
#include "settings.h"

void boot_init(void){
	lcd_print("STTNG:A  KEYPD:B");
	lcd_pri
	keypad_set_read_callback(boot);
	BOOT_MODE = 0;
}

void settings_init(void){
	
}

void boot(int row, int col){
	switch (BOOT_MODE){
		case 0:
			BOOT_MODE=1;
			keypad_set_read_callback(settings);
			settings_init();
			break;
		case 1:
			break;
		case 2:
			keypad
	}
}
