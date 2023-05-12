#include "tone.h"
#include "delay.h"
#include "lcd.h"
#include "dtmf_symbols.h"
#include "keypad.h"
#include <platform.h>
#include <lpc_eeprom.h>

Settings Current;
	
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
