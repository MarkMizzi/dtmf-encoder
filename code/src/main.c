#include <platform.h>
#include "tone.h"
#include "delay.h"
#include "lcd.h"

// Comment to play with interrupts, uncomment to play with blocking.
//#define BUSY_WAIT

int main(void) {

	lcd_init();
	lcd_clear();
	
	tone_init();
	__enable_irq();
	
		tone_play_with_interrupt(0, 2);

	lcd_put_char('a');
	delay_ms(3000);
	
	tone_play_with_interrupt(2, 1);
	while(1);
}

// *******************************ARM University Program Copyright © ARM Ltd 2014*************************************   
