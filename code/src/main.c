#include "dac_interrupt.h"
#include "dtmf_symbols.h"
#include "queue.h"

#include <dac.h>

void start_or_enqueue(int symbol);

void start_or_enqueue(int symbol) {
	if (!dac_interrupt_enable(COL(symbol), ROW(symbol))) {
		enqueue(symbol);
	}
}

int main(void) {
	dac_init();
	__enable_irq();
	start_or_enqueue(SYMBOL_0);
	while (1);
}
