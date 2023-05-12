#include "tone.h"
#include "delay.h"
#include "lcd.h"
#include "dtmf_symbols.h"
#include "keypad.h"
#include "settings.h"
#include <lpc_eeprom.h>
#include <platform.h>

Settings current;

#define PCTIM1 2
#define PCUART0 3
#define PCUART1 4
#define PCI2C0 7
#define PCI2C1 19
#define PCI2C2 26
#define PCRTC 9
#define PCGPIO 15

void power_down_peripherals()
{
	LPC_SC->PCONP &= ~(1 << PCTIM1);
	LPC_SC->PCONP &= ~(1 << PCUART0);
	LPC_SC->PCONP &= ~(1 << PCUART1);
	LPC_SC->PCONP &= ~(1 << PCI2C0);
	LPC_SC->PCONP &= ~(1 << PCI2C1);
	LPC_SC->PCONP &= ~(1 << PCI2C2);
	LPC_SC->PCONP &= ~(1 << PCRTC);
}

int main(void) {
	
	lcd_init();
	lcd_clear();
	EEPROM_Init();
	__enable_irq();
	
	current.inter_symbol_spacing = DEFAULT_INTER_SYMBOL_SPACING_MS;
	current.symbol_length = DEFAULT_SYMBOL_LENGTH_MS;
	current.sampling_rate_multiplier = DEFAULT_SAMPLING_RATE_MULTIPLIER;
	
	EEPROM_Write(0, 0, &current, MODE_16_BIT, sizeof(Settings)>>1);
	
	settings_init();
	
	keypad_init();	
	
	tone_init();
	
	while (1) {
		__WFI();
	}
}
