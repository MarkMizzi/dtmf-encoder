#include "tone.h"
#include "delay.h"
#include "lcd.h"
#include "dtmf_symbols.h"
#include "keypad.h"
#include <platform.h>
#include <gpio.h>

void keypad_callback(int sources) {
	uint32_t pin_mask;
	
	gpio_set(P_DBG_ISR, 1);
	
	pin_mask = 
		(1 << GET_PIN_INDEX(P_ROW_0)) |
		(1 << GET_PIN_INDEX(P_ROW_1)) |
		(1 << GET_PIN_INDEX(P_ROW_2)) |
		(1 << GET_PIN_INDEX(P_ROW_3));

	if (!(sources & pin_mask)) {
		return;
	}
	
	lcd_put_char('A');
	gpio_set(P_DBG_ISR, 1);
}
	

int main(void) {
	
	LPC_SC->PCONP &= ~(1 << 12); // Clear bit 12 to disable ADC
	
	keypad_set_read_callback(tone_play_or_enqueue);
	
	volatile int i = 0;
	
	lcd_init();
	lcd_clear();
	
	tone_init();
	__enable_irq();
	
	keypad_init();
	
	while (1) {
		gpio_toggle(P_DBG_MAIN);
	}
}