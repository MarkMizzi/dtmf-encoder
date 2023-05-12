#ifndef SETTINGS_H
#define SETTINGS_H

#include "lpc_types.h"

#define SETTINGS_PAGE 0
#define SETTINGS_OFFSET 0

#define DEFAULT_INTER_SYMBOL_SPACING_MS 200
#define DEFAULT_SYMBOL_LENGTH_MS 500
#define DEFAULT_SAMPLING_RATE_MULTIPLIER 2

typedef struct Settings {
	uint16_t inter_symbol_spacing;
	uint16_t symbol_length;
	uint16_t sampling_rate_multiplier;
	uint16_t checksum;
} Settings;

void enter_boot_mode(void);
void boot_menu_input(int row, int col);

void enter_settings_mode(void);
void settings_menu_input(int row, int col);

void enter_quickdial_mode(void);
void quickdial_menu_input(int row, int col);

void load_settings();
void store_settings();

extern Settings settings;

#endif