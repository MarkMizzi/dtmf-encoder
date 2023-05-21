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
*\brief NewProfileStage Enumerated list to store each stage of creating a new profile
*/
enum NewProfileStage {
	PickProfile = 0,
	SetISS = 1,
	SetSymbolLength = 2,
	SetQuality = 3,
	SetProfileLength = 4,
};

/**
*stage Stores what current stage profile creation is in.
*/
static int stage = 0;
/**
*profile_num Stores what profile is being created, loaded or deleted.
*/
static int profile_num;
static Profile curr_profile;
void set_setting_input(int row, int col);
void del_profile(int row, int col);
void set_characters();
int checksum_check(Profile profile);

#define SYMBOL_TO_NUM(SYMBOL) \
	((int) symbol_chars[SYMBOL]) - '0'
		
#define PROFILE_PAGE(PROFILE_NUM) \
	(SETTINGS_PAGE + (PROFILE_NUM) + 1)
	
#define PROFILE_OFFSET 0

/**
* \brief load_profile Loads a profile from the EEPROM, performs bounds checking queues the tones into the queue. Once done exits back to boot menu.
*/
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

/**
* \brief quickdial_init Creates the initial menu for quickdial and sets the ISR callback appropriately.
*/
void quickdial_init(void){
	lcd_clear();
	lcd_print("A:NEW      B:DEL");
	lcd_set_cursor(0, 1);
	lcd_print("0-9: SEL PROFILE");
	keypad_set_read_callback(quickdial_menu_input);
}

/**
* \brief quickdial_menu_input Deals with handling the input for deleting, loading and creating profiles.
* @param row Row of key that caused the ISR
* @param col Column of key that caused the ISR
*/
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
/**
* \brief Deletes a given profile by rewriting the page in the EEPROM with zeros
*/
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
/**
* \brief set_setting_input Deals with handling the input for setting setiings for new profiles. Performs bounds checking for each input.
* @param row Row of key that caused the ISR
* @param col Column of key that caused the ISR
*/
void set_setting_input(int row, int col) {
	Settings settings;
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
						stage = 0;
					} else {
						setting_val = 0;
						clear_user_input();
					}
					
					break;
			}
			break;
		
		case SYMBOL_STAR:
			setting_val = 0;
			clear_user_input();
			break;
	}
}

/**
* \brief set_setting_input Deals with handling the input for setting the saved characters for new profiles.
* @param row Row of key that caused the ISR
* @param col Column of key that caused the ISR
*/
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

/**
* \brief calculates the checksum of a Profile. Used on creation and on loading for verification to ensure no errors in case of EEPROM failure.
*/
int checksum_check(Profile profile){
	int i;
	uint16_t calc_checksum = profile.length ^ SETTINGS_CHECKSUM(profile.settings);
	
	for (i = 0; i < profile.length; i++){
		calc_checksum = calc_checksum ^ profile.profile_characters[i];
	}
	
	return calc_checksum;
}