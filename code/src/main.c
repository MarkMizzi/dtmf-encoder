#include "tone.h"
#include "delay.h"
#include "lcd.h"
#include "dtmf_symbols.h"
#include "keypad.h"
#include "settings.h"
#include <lpc_eeprom.h>
#include <platform.h>

/** \brief Index of power bit for Timer 1 peripheral in `LPC_SC->PCONP`.
 */
#define PCTIM1 2
/** \brief Index of power bit for UART 0 peripheral in `LPC_SC->PCONP`.
 */
#define PCUART0 3
/** \brief Index of power bit for UART 1 peripheral in `LPC_SC->PCONP`.
 */
#define PCUART1 4
/** \brief Index of power bit for I2C controller 0 peripheral in `LPC_SC->PCONP`.
 */
#define PCI2C0 7
/** \brief Index of power bit for I2C controller 1 peripheral in `LPC_SC->PCONP`.
 */
#define PCI2C1 19
/** \brief Index of power bit for I2C controller 2 peripheral in `LPC_SC->PCONP`.
 */
#define PCI2C2 26
/** \brief Index of power bit for RTC timer peripheral in `LPC_SC->PCONP`.
 */
#define PCRTC 9

/** \brief Powers down unneeded peropherals.
 *
 * This is called at system start-up in order to maximize power efficiency.
 * Only peripherals which are initially on are considered.
 */
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
