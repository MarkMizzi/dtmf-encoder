#include "quickdial.h"
#include "lcd.h"
#include "keypad.h"
#include "settings.h"
#include "menu.h"
#include "dtmf_symbols.h"
#include "lpc_eeprom.h"
#include "delay.h"
#include "tone.h"
#include <string.h>


/**
 * \brief Enum representing stages in creating a new profile. 
 * 
 * Used by #set_setting_input to keep track of current stage while prompting the user for profile fields.
 */
enum NewProfileStage {
	PickProfile = 0,
	SetISS = 1,
	SetSymbolLength = 2,
	SetQuality = 3,
	SetProfileLength = 4,
};

/**
 * \brief Stores the current stage which profile creation is in.
 */
static enum NewProfileStage stage = PickProfile;

/**
 * \brief Stores what profile is being created or loaded.
 */
static int profile_num;

/**
 * \brief Used to hold data for a profile while it is being created or played back.
 */
static Profile curr_profile;

/**
 * \brief Loads a profile from the EEPROM, performs bounds checking and plays back the tone. 
 *
 * Once playback is over, the user is redirected back to boot menu.
 * A busy waiting delay is used to ensure that the profile finishes playback before this occurs.
 */
void load_profile(int symbol);

/**
 * \brief Handles user input for setting fields of a new profile. 
 *
 * User input proceeds in stages, with one stage for each field of the new profile.
 * Bounds checking is performed before proceeding to the next stage. If the user input is invalid, it is cleared, and
 * they are prompted to enter a new value for the same field.
 * 
 * \param row Row of key press
 * \param col Column of key press
 */
void set_setting_input(int row, int col);

/**
 * \brief Deletes a given profile by zeroing out the corresponding region in the EEPROM containing that profile.
 *
 * \param row Row of key press indicating profile to be deleted.
 * \param col Column of key press indicating profile to be deleted.
 */
void del_profile(int row, int col);

/**
 * \brief Handles user input for setting the saved characters of a new profile.
 *
 * User input is registered until the number of entered characters is the same as the length indicated in #curr_profile
 *
 * \param row Row of key press
 * \param col Column of key press
 */
void set_characters(int row, int col);

/**
 * \brief Calculates the checksum of a #Profile and compares it to its `checksum` field. 
 *
 * Used on loading a profile to ensure that the profile has not been corrupted.
 *
 * \param profile #Profile whose checksum is being verified.
 */
int checksum_check(Profile profile);

/**
 * \brief Converts a numeric DTMF symbol to its corresponding digit value.
 *
 * Using a non-numeric DTMF symbol with this macro returns an invalid result.
 *
 * \param SYMBOL Numeric DTMF symbol to be converted.
 */
#define SYMBOL_TO_NUM(SYMBOL) \
	((int) symbol_chars[SYMBOL]) - '0'

/**
 * \brief Gets the EEPROM page used to store a particular profile.
 *
 * \param PROFILE_NUM Number of profile whose page has been requested.
 */	
#define PROFILE_PAGE(PROFILE_NUM) \
	(SETTINGS_PAGE + (PROFILE_NUM) + 1)

/**
 * \brief Gets the offset inside an EEPROM page where a profile is stored. This is the same for each profile.
 */	
#define PROFILE_OFFSET 0

void load_profile(int symbol){
	int i;
	int profile_num = SYMBOL_TO_NUM(symbol);
	EEPROM_Read(PROFILE_OFFSET, PROFILE_PAGE(profile_num), (void*)&curr_profile, MODE_16_BIT, sizeof(Profile) >> 1);
	
	if (checksum_check(curr_profile) == curr_profile.checksum &&
		  curr_profile.length >= MIN_PROFILE_LENGTH &&
		  curr_profile.length <= MAX_PROFILE_LENGTH &&
		  curr_profile.settings.lut_logsize >= MIN_LUT_LOGSIZE &&
		  curr_profile.settings.lut_logsize <= MAX_LUT_LOGSIZE &&
		  curr_profile.settings.symbol_length >= MIN_SYMBOL_LENGTH_MS &&
		  curr_profile.settings.symbol_length <= MAX_SYMBOL_LENGTH_MS &&
		  curr_profile.settings.inter_symbol_spacing >= MIN_INTER_SYMBOL_SPACING_MS &&
		  curr_profile.settings.inter_symbol_spacing <= MAX_INTER_SYMBOL_SPACING_MS){
		lcd_clear();
		settings = curr_profile.settings;
		tone_init();
				
		for (i = 0; i < curr_profile.length; i++){
			tone_play_or_enqueue(ROW(curr_profile.profile_characters[i]), COL(curr_profile.profile_characters[i]));
		}
		
		delay_ms(curr_profile.length * curr_profile.settings.symbol_length + (curr_profile.length-1)*curr_profile.settings.inter_symbol_spacing+500);
		boot_mode_init();
		
	} else {
		lcd_print("LOADING FAILED");
		delay_ms(2000);
		boot_mode_init();
	}
}

void quickdial_init(void){
	lcd_clear();
	lcd_print("A:NEW      B:DEL");
	lcd_set_cursor(0, 1);
	lcd_print("0-9: SEL PROFILE");
	keypad_set_read_callback(quickdial_menu_input);
}

void quickdial_menu_input(int row, int col){
	switch (SYMBOL(row, col)){
		case SYMBOL_0:
		case SYMBOL_1:
		case SYMBOL_2:
		case SYMBOL_3:
		case SYMBOL_4:
		case SYMBOL_5:
		case SYMBOL_6:
		case SYMBOL_7:
		case SYMBOL_8:
		case SYMBOL_9:
			load_profile(SYMBOL(row, col));
			break;
			
		case SYMBOL_A:
			lcd_clear();
			lcd_print("NEW PROFILE 0-9");
			keypad_set_read_callback(set_setting_input);
			break;
		
		case SYMBOL_B:
			lcd_clear();
			lcd_print("DEL PROFILE 0-9");
			keypad_set_read_callback(del_profile);
			break;
		
			
	}
}

void del_profile(int row, int col){
	int zero_array[sizeof(Profile)] = {0};
	int prof;
	
	switch (SYMBOL(row, col)){
		case SYMBOL_0:
		case SYMBOL_1:
		case SYMBOL_2:
		case SYMBOL_3:
		case SYMBOL_4:
		case SYMBOL_5:
		case SYMBOL_6:
		case SYMBOL_7:
		case SYMBOL_8:
		case SYMBOL_9:
			prof = SYMBOL_TO_NUM(SYMBOL(row, col));
			EEPROM_Write(PROFILE_OFFSET, PROFILE_PAGE(prof), (void*)&zero_array, MODE_16_BIT, sizeof(Profile) >> 1);
			boot_mode_init();
	}
}

void set_setting_input(int row, int col) {
	static int setting_val = 0;

	int symbol = SYMBOL(row, col);	
	switch (symbol) {
		case SYMBOL_0:
		case SYMBOL_1:
		case SYMBOL_2:
		case SYMBOL_3:
		case SYMBOL_4:
		case SYMBOL_5:
		case SYMBOL_6:
		case SYMBOL_7:
		case SYMBOL_8:
		case SYMBOL_9:
			if (stage == PickProfile){
					profile_num = SYMBOL_TO_NUM(symbol);
					stage++;
					lcd_clear();
					display_menu_options();
					menu_prompt("ISS:");
			} else {
					lcd_put_char(symbol_chars[symbol]);
					keypad_input_to_number(row, col, &setting_val);
			}
			
			break;
		
		case SYMBOL_POUND:
			switch (stage){
				case SetISS:
					if (setting_val >= MIN_INTER_SYMBOL_SPACING_MS &&
				      setting_val <= MAX_INTER_SYMBOL_SPACING_MS) {
						curr_profile.settings.inter_symbol_spacing = setting_val;
						setting_val = 0;
						
						stage++;
						
						lcd_clear();
						display_menu_options();
						menu_prompt("SYMLEN:");
					} else {
						setting_val = 0;
						clear_user_input();
					}			
					
					break;
					
				case SetSymbolLength:
					if (setting_val >= MIN_SYMBOL_LENGTH_MS &&
							setting_val <= MAX_SYMBOL_LENGTH_MS) {
						curr_profile.settings.symbol_length = setting_val;
						setting_val = 0;
					
						stage++;
						
						lcd_clear();
						display_menu_options();
						menu_prompt("QUALITY:");
					} else {
						setting_val = 0;
						clear_user_input();
					}
					
					break;
					
				case SetQuality:
					if (setting_val >= MIN_LUT_LOGSIZE &&
				      setting_val <= MAX_LUT_LOGSIZE) {
						curr_profile.settings.lut_logsize = setting_val;
						setting_val = 0;
					
						stage++;
					
						lcd_clear();
						display_menu_options();
						menu_prompt("PROFILE LEN:");
					} else {
						setting_val = 0;
						clear_user_input();
					}
					
					break;
					
				case SetProfileLength:
					if (setting_val >= MIN_PROFILE_LENGTH &&
				      setting_val <= MAX_PROFILE_LENGTH) {
						curr_profile.length = setting_val;
						setting_val = 0;
						
						lcd_clear();
						keypad_set_read_callback(set_characters);
						stage = PickProfile;
					} else {
						setting_val = 0;
						clear_user_input();
					}
					
					break;
				default:
					break;
			}
			break;
		
		case SYMBOL_STAR:
			setting_val = 0;
			clear_user_input();
			break;
	}
}

void set_characters(int row, int col){
	static int i = 0;
	static int checksum = 0;
	
	lcd_put_char(symbol_chars[SYMBOL(row, col)]);
	curr_profile.profile_characters[i] = SYMBOL(row, col);
	checksum = checksum ^ SYMBOL(row, col);

	if (i < curr_profile.length - 1){
		++i;
	} else {
		checksum = checksum ^ curr_profile.length ^ SETTINGS_CHECKSUM(curr_profile.settings);
		curr_profile.checksum = checksum;

		EEPROM_Write(PROFILE_OFFSET, PROFILE_PAGE(profile_num), (void*)&curr_profile, MODE_16_BIT, sizeof(Profile) >> 1);

		i = 0;
		checksum = 0;

		lcd_set_cursor_visibile(0);
		memset((void *)&curr_profile, 0, sizeof(Profile));
		boot_mode_init();
	}
}

int checksum_check(Profile profile){
	int i;
	uint16_t calc_checksum = profile.length ^ SETTINGS_CHECKSUM(profile.settings);
	
	for (i = 0; i < profile.length; i++){
		calc_checksum = calc_checksum ^ profile.profile_characters[i];
	}
	
	return calc_checksum;
}
