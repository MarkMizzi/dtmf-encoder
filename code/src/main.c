#include "dac_interrupt.h"
#include "dtmf_symbols.h"
#include "queue.h"

#include <dac.h>
#include <system_LPC407x_8x_177x_8x.h>

void start_or_enqueue(int symbol);

void start_or_enqueue(int symbol) {
	if (!dac_interrupt_enable(COL(symbol), ROW(symbol))) {
		enqueue(symbol);
	}
}

int main(void) {
	volatile int i;
	dac_init();
	__enable_irq();
	start_or_enqueue(SYMBOL_0);
	for (i = 0; i < 1000000; i++);
	start_or_enqueue(SYMBOL_8);
	for (i = 0; i < 10000000; i++);
	start_or_enqueue(SYMBOL_STAR);
	while (1);
}
