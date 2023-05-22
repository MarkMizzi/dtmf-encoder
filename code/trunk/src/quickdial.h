#ifndef QUICKDIAL_H
#define QUICKDIAL_H

#include "lcd.h"
#include "keypad.h"
#include "settings.h"
#include "menu.h"
#include "dtmf_symbols.h"
#include "lpc_types.h"

/**
 * \brief Minimum length for a quickdial profile.
 */
#define MIN_PROFILE_LENGTH 1
/**
 * \brief Maximum length for a quickdial profile.
 */
#define MAX_PROFILE_LENGTH 32


/**
 * \brief Struct used to represent a quickdial profile
 */
typedef struct Profile {
	/**
	 * \brief Settings for the quickdial profile.
	 */
	Settings settings;
	/**
	 * \brief XOR checksum used to verify integrity of a #Profile loaded from EEPROM.
	 */
	uint16_t checksum;
	/**
	 * \brief Length (in number of symbols) of a profile.
	 */
	uint8_t length;
	/**
	 * \brief Array of symbols in the profile. 
	 *
	 * Initialized to zero, and filled with as many characters as indicated by the profile length.
	 */
	char profile_characters[MAX_PROFILE_LENGTH];
} Profile;

/**
* \brief Creates the initial menu for quickdial and sets #keypad_read_callback appropriately.
*/
void quickdial_init(void);

/**
 * \brief Handles user input for the quickdial menu: i.e. choosing between deleting, loading and creating profiles.
 *
 * \param row Row of the key pressed
 * \param col Column of the key pressed
 */
void quickdial_menu_input(int row, int col);

#endif
