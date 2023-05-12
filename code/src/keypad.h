#ifndef KEYPAD_H
#define KEYPAD_H

#include <platform.h>

//defining macros for the pins connected to the columns
//32 - 29 columns
/**
 * \brief Macros for column pins having positions 0, 1, 2, 3 
 */
#define P_COL_0 P1_11
#define P_COL_1	P1_5
#define P_COL_2 P1_6
#define P_COL_3 P1_7

//defining macros for the pins connected to the rows
//28 - 25 rows
/**
 * \brief Macros for row pins having positions 0, 1, 2, 3 
 */
#define P_ROW_0 P0_21
#define P_ROW_1 P0_0
#define P_ROW_2 P0_24
#define P_ROW_3 P0_25

/**
 * \brief Macro for the pin that triggers the interrupt on the falling edge
 * 				The interrupt triggers the read_keypad() function
 */
#define P_INTERRUPT P2_10

/**
 * \brief Macro defining the total number of keypad rows
 */
#define KEYPAD_ROWS 4
/**
 * \brief Macro defining the total number of keypad columns
 */
#define KEYPAD_COLS 4

/**
 * \brief Indexes the appropriate column pin using the column position number
 */
static Pin colno_to_pin[KEYPAD_COLS] = {P_COL_0, P_COL_1, P_COL_2, P_COL_3};
/**
 * \brief Indexes the appropriate row pin using the row position number
 */
static Pin rowno_to_pin[KEYPAD_ROWS] = {P_ROW_0, P_ROW_1, P_ROW_2, P_ROW_3};

/**
 * \brief Initializes the keypad driver code
 *
 *				The column pins are set as output and low using the gpio_set_mode() and gpio_set() functions
 *				The row pins are set as input and are pulled up using the gpio_set_mode() and gpio_set() functions
 *				The #P_INTERRUPT pin is pulled up and set as an interrupt pin using the functions gpio_set_mode() gpio_set_trigger()
 *				The function called on hardware interrupt is set using the gpio_set_callback() functions
 *
 * @param col The column of the symbol whose tone is to be generated.
 * @param row The row of the symbol whose tone is to be generated.
 */
void keypad_init(void);
/**
 * \brief Sets the read_keypad_callback() function which is called in the read_keypad() function
 * @param callback The callback function used to set read_keypad_callback()
 */
void keypad_set_read_callback(void (*callback)(int, int));

#endif // KEYPAD_H
