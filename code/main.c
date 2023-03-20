#include "dac_interrupt.h"
#include "dtmf_symbols.h"
#include "queue.h"

int main(void);
void start_or_enqueue(int symbol);

void start_or_enqueue(int symbol) {
	if (!dac_interrupt_enable(COL(symbol), ROW(symbol))) {
		enqueue(symbol);
	}
}

int main(void) {
	start_or_enqueue(SYMBOL_0);
	for (volatile int i = 0; i < 1000000; i++);
	start_or_enqueue(SYMBOL_6);
	for (;;);
}
