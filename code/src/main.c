#include <platform.h>
#include "tone.h"
#include "delay.h"
#include "lcd.h"
#include "dtmf_symbols.h"
#include "keypad.h"

// Comment to play with interrupts, uncomment to play with blocking.
//#define BUSY_WAIT

int main(void) {

	lcd_init();
	lcd_clear();
	keypad_init();
	
	tone_init();
	__enable_irq();
	
	while (1) {
		read_keypad(tone_play_or_enqueue);
	}
}

// *******************************ARM University Program Copyright © ARM Ltd 2014*************************************   
