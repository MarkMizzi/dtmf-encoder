#ifndef SETTINGS_H
#define SETTINGS_H

#include "lpc_types.h"

/** \brief Page containing saved system settings
 */
#define SETTINGS_PAGE 0
/** \brief Offset of saved system settings in #SETTINGS_PAGE
 */
#define SETTINGS_OFFSET 0

/** \brief Default value for inter-symbol spacing.
 */
#define DEFAULT_INTER_SYMBOL_SPACING_MS 200

/** \brief Default value for symbol length.
 */
#define DEFAULT_SYMBOL_LENGTH_MS 500

/** \brief Default value for log base 2 of the size of #sine_table.
 */
#define DEFAULT_LUT_LOGSIZE 5

/** \brief Minimum value for inter-symbol spacing.
 */
#define MIN_INTER_SYMBOL_SPACING_MS 0

/** \brief Minimum value for symbol length.
 */
#define MIN_SYMBOL_LENGTH_MS 50

/** \brief Minimum value for log base 2 of the size of #sine_table.
 */
#define MIN_LUT_LOGSIZE 4

/** \brief Maximum value for inter-symbol spacing.
 */
#define MAX_INTER_SYMBOL_SPACING_MS 5000

/** \brief Maximum value for symbol length.
 */
#define MAX_SYMBOL_LENGTH_MS 5000

/** \brief Maximum value for log base 2 of the size of #sine_table.
 */
#define MAX_LUT_LOGSIZE 9

/** \brief XOR checksum computed on fields of a #Settings struct.
 *
 * This is used as a basic integrity check for settings loaded from EEPROM.
 */
#define SETTINGS_CHECKSUM(SETTINGS) \
	((SETTINGS).inter_symbol_spacing ^ (SETTINGS).symbol_length ^ (SETTINGS).lut_logsize)

/** \brief Struct used to represent system settings.
 * 
 * Since this struct is directly serialized and stored to EEPROM as raw bytes, it is important that
 * it contains only flat fields (no pointers).
 */
typedef struct Settings {
	uint16_t inter_symbol_spacing;
	uint16_t symbol_length;
	uint16_t lut_logsize;
	uint16_t checksum;
} Settings;

/** \brief When invoked, the system enters boot mode.
 *
 * In boot mode, the user is presented with the option to choose between one 
 * of three system modes: Manual DTMF encoding, quickdial, and settings mode.
 */
void boot_mode_init(void);

/** \brief Callback to process option selected by user in boot mode.
 *
 * \param row Row of the key pressed.
 * \param col Column of the key pressed.
 */
void boot_menu_input(int row, int col);

/** \brief When invoked, the system enters settings mode.
 * 
 * In settings mode, the user is presented with the option to set the
 * inter-symbol spacing, symbol length, and quality of tone produced by the system.
 */
void settings_mode_init(void);
/** \brief Callback to process option selected by user in settings mode.
 *
 * \param row Row of the key pressed.
 * \param col Column of the key pressed.
 */
void settings_menu_input(int row, int col);

/** \brief When invoked, the system enters a mode allowing the user to input a value for the
 * inter-symbol spacing.
 */
void set_inter_symbol_spacing_mode_init(void);
/** \brief Processes user input in the mode where the user can specify inter-symbol spacing.
 *
 * An input of `*` clears user input, whereas `#` applies the user input.
 * Digit characters are accumulated using a static function variable.
 * Invalid characters are ignored silently.
 *
 * \param row Row of the key pressed.
 * \param col Column of the key pressed.
 */
void set_inter_symbol_spacing_menu_input(int row, int col);

/** \brief When invoked, the system enters a mode allowing the user to input a value for the
 * symbol length.
 */
void set_symbol_length_mode_init(void);
/** \brief Processes user input in the mode where the user can specify symbol length.
 *
 * An input of `*` clears user input, whereas `#` applies the user input.
 * Digit characters are accumulated using a static function variable.
 * Invalid characters are ignored silently.
 *
 * \param row Row of the key pressed.
 * \param col Column of the key pressed.
 */
void set_symbol_length_menu_input(int row, int col);

/** \brief When invoked, the system enters a mode allowing the user to input a value for the
 * log base 2 of the size of #sine_table.
 */
void set_lut_logsize_mode_init(void);
/** \brief Processes user input in the mode where the user can specify log base 2 of the size of #sine_table.
 *
 * An input of `*` clears user input, whereas `#` applies the user input.
 * Digit characters are accumulated using a static function variable.
 * Invalid characters are ignored silently.
 *
 * \param row Row of the key pressed.
 * \param col Column of the key pressed.
 */
void set_lut_logsize_menu_input(int row, int col);

/** \brief Perform bounds checking on fields of #settings, and set out-of-bounds fields to default values.
 *
 * \param settings Pointer to instance of #Settings to be bounds-checked.
 */
void check_settings(Settings *settings);
/** \brief Load settings from the EEPROM storage, and sets #settings to the loaded version.
 * 
 * Before setting #settings, the entire loaded struct is checked using the checksum, and each field
 * is checked separately to see if it is out of bounds.
 * Any invalid fields are set to the default value for that field.
 */
void load_settings();
/** \brief Store settings to EEPROM. The checksum is computed before storing.
 */
void store_settings();

/** \brief Global variable containing current system settings.
 */
extern Settings settings;

#endif