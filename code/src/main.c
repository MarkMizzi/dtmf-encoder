#include "dac_interrupt.h"
#include "dtmf_symbols.h"
#include "queue.h"
#include "lcd.h"

#include <dac.h>
#include <system_LPC407x_8x_177x_8x.h>

char char_array[16] = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D'};

void start_or_enqueue(int symbol);

void start_or_enqueue(int symbol) {
	if (!dac_interrupt_enable(COL(symbol), ROW(symbol))) {
		enqueue(symbol);
	}
	lcd_put_char(char_array[symbol]);
}

int main(void) {
	volatile int i;
	dac_init();
	lcd_init();
	lcd_clear();
	__enable_irq();
	
	start_or_enqueue(SYMBOL_0);
	for (i = 0; i < 100000000; i++);
	start_or_enqueue(SYMBOL_8);
	for (i = 0; i < 100000000; i++);
	start_or_enqueue(SYMBOL_STAR);
	while (1);
}
