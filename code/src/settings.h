#ifndef SETTINGS_H
#define SETTINGS_H

#include "lpc_types.h"

extern int BOOT_MODE;
extern int rate_multiplier;
extern int symbol_length_ms;
extern int intersymbol_spacing_ms;

typedef struct settings{
	uint16_t inter_symbol_spacing;
	uint16_t symbol_length;
	uint16_t sampling_rate_multiplier;
}Settings;

void boot_init(void);
void boot(int row, int col);
void settings();
void settings_init(void);

#endif