#include "tone.h"
#include "delay.h"
#include "lcd.h"
#include "dtmf_symbols.h"
#include "keypad.h"
#include <platform.h>

#define PCTIM1 2
#define PCUART0 3
#define PCUART1 4
#define PCI2C0 7 // - may need it for the DAC
#define PCRTC 9
#define PCI2C1 19 // - may need it for the DAC
#define PCI2C2 26 // - may also need it for the DAC

void power_down_peripherals()
{
	LPC_SC->PCONP = ~(1 << PCTIM1);
	LPC_SC->PCONP = ~(1 << PCUART0);
	LPC_SC->PCONP = ~(1 << PCUART1);
	LPC_SC->PCONP = ~(1 << PCI2C0);
	LPC_SC->PCONP = ~(1 << PCI2C1);
	LPC_SC->PCONP = ~(1 << PCI2C2);
	LPC_SC->PCONP = ~(1 << PCRTC);
}

int main(void) {
	power_down_peripherals();
	keypad_set_read_callback(tone_play_or_enqueue);
	
	lcd_init();
	lcd_clear();
	
	tone_init();
	__enable_irq();
	
	keypad_init();
	
	while (1) {
		__WFI();
	}
}
