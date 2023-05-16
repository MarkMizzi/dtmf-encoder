#ifndef QUICKDIAL_H
#define QUICKDIAL_H

#include "lcd.h"
#include "keypad.h"
#include "settings.h"
#include "menu.h"
#include "dtmf_symbols.h"
#include "lpc_types.h"

typedef struct profile{
	Settings settings;
	uint16_t checksum;
	uint8_t length;
	char profile_characters[32];
}Profile;

void quickdial_init(void);
void quickdial_menu_input(int row, int col);

#endif