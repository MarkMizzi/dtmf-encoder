#pragma once

#include <stdbool.h>

// NOTE: for mocking
#define SYMBOL_LENGTH 2

bool dac_interrupt_enable(int col, int row);
void dac_interrupt_disable(void);