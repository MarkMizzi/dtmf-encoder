#include <platform.h>
#include "tone.h"
#include "delay.h"
#include "lcd.h"
#include "dtmf_symbols.h"
#include "keypad.h"

// Comment to play with interrupts, uncomment to play with blocking.
//#define BUSY_WAIT

int main(void) {

	volatile int i = 0;
	
	lcd_init();
	lcd_clear();
	
	for(i=0; i < 64; i++){
		lcd_put_char('A');
		delay_ms(200);
	}
}

// *******************************ARM University Program Copyright © ARM Ltd 2014*************************************   
