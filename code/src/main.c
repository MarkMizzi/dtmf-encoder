#include "tone.h"
#include "delay.h"
#include "lcd.h"
#include "dtmf_symbols.h"
#include "keypad.h"
#include <platform.h>
#include <gpio.h>

int main(void) {
	
	keypad_set_read_callback(tone_play_or_enqueue);
	
	lcd_init();
	lcd_clear();
	
	tone_init();
	__enable_irq();
	
	keypad_init();
	
	while (1) {
		__asm("WFI");
	}
}
