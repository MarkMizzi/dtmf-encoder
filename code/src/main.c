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
	power_down_peripherals();
	
	lcd_init();
	lcd_clear();
	EEPROM_Init();
	__enable_irq();
	
	keypad_init();
	
	boot_mode_init();
	
	while (1) {
		__WFI();
	}
}
