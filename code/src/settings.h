#include "lpc_types.h"

int BOOT_MODE;

typedef struct settings{
	uint16_t inter_symbol_spacing;
	uint16_t symbol_length;
	uint16_t sampling_rate_multiplier;
}Settings;

void boot_init(void);
void boot(int row, int col);
void settings();