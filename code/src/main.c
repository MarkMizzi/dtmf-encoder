#include "tone.h"
#include "delay.h"
#include "lcd.h"
#include "dtmf_symbols.h"
#include "keypad.h"
#include <platform.h>
#include <lpc_eeprom.h>

Settings Current;
	

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
	
	Current.InterSymbolSpacing=200;
	Current.SymbolLength=500;
	Current.SamplingRateMultiplier=2;
	EEPROM_Write(0, 0, &Current, MODE_16_BIT, sizeof(Settings)>>1);
	
	keypad_init();	
	
	tone_init();
	
	while (1) {
		__asm("WFI");
	}
}
