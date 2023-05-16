#ifndef MENU_H
#define MENU_H

/** \brief Displays OK and Clear options for a menu that processes user input.
 *
 * For such menus, the `*` key is used to clear user input, 
 * while the `#` key is used to submit user input.
 */
void display_menu_options();

/** \brief Displays a prompt for user input. Used in menus that process user input.
 *
 * \param prompt Format string for the prompt
 * \param ... Parameters to paste into format string.
 */
void menu_prompt(const char *prompt, ...);

/** \brief Clears user input from the screen, used in menus that process user input.
 *
 * The cursor is reset to its position before any user input.
 */
void clear_user_input();

/** \brief Processes a single digit input by the user, adding it to the value in #curr.
 *
 * The old value pointed to by #curr is multiplied by 10 before adding the newly processed digit.
 * This allows a user to enter a number digit by digit on the keypad, with the system
 * storing its numerical value in #curr.
 * 
 * \param row The row of the keypad pressed to enter the new digit.
 * \param col The column of the keypad pressed to enter the new digit.
 * \param curr Pointer to location where the value of the new digit will be stored.
 */
void keypad_input_to_number(int row, int col, int *curr);


#endif