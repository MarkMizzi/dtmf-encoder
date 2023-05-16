#ifndef QUICKDIAL_H
#define QUICKDIAL_H

#include "lcd.h"
#include "keypad.h"
#include "settings.h"
#include "menu.h"
#include "dtmf_symbols.h"
#include "lpc_types.h"

#define MAX_PROFILE_LENGTH 32

typedef struct Profile {
	Settings settings;
	uint16_t checksum;
	uint8_t length;
	char profile_characters[MAX_PROFILE_LENGTH];
} Profile;

void quickdial_init(void);
void quickdial_menu_input(int row, int col);

#endif