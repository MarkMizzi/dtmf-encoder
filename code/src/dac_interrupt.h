#pragma once

#include <stdbool.h>

#define SYMBOL_LENGTH 1
#define INTERSYMBOL_SPACING 0.5

bool dac_interrupt_enable(int col, int row);
void dac_interrupt_disable(void);
