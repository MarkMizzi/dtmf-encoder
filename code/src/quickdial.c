#include "quickdial.h"
#include "lcd.h"
#include "keypad.h"
#include "settings.h"
#include "menu.h"
#include "dtmf_symbols.h"
#include "lpc_eeprom.h"
#include "delay.h"
#include "tone.h"

static int stage = 0;
static int profile_num;
static Profile curr_profile;
void set_setting_input(int row, int col);
void del_profile(int row, int col);
void set_characters();
int checksum_check(Profile profile);

#define SYMBOL_TO_NUM(SYMBOL) \
	((int) symbol_chars[SYMBOL]) - '0'

void load_profile(int x){
	int i;
	x = SYMBOL_TO_NUM(x);
	EEPROM_Read(SETTINGS_PAGE + x + 1, SETTINGS_OFFSET, (void*)&curr_profile, MODE_16_BIT, sizeof(Profile) >> 1);
	
	if (checksum_check(curr_profile) == curr_profile.checksum &&
		curr_profile.length >= 0 &&
		curr_profile.settings.lut_logsize >= MIN_LUT_LOGSIZE &&
		curr_profile.settings.lut_logsize <= MAX_LUT_LOGSIZE &&
		curr_profile.settings.symbol_length >= MIN_SYMBOL_LENGTH_MS &&
		curr_profile.settings.symbol_length <= MIN_SYMBOL_LENGTH_MS &&
		curr_profile.settings.inter_symbol_spacing >= MIN_INTER_SYMBOL_SPACING_MS &&
		curr_profile.settings.inter_symbol_spacing <= MAX_INTER_SYMBOL_SPACING_MS){
		lcd_clear();
		settings = curr_profile.settings;
		tone_init();
		for (i = 0; i < curr_profile.length; i++){
			tone_play_or_enqueue(ROW(curr_profile.profile_characters[i]), COL(curr_profile.profile_characters[i]));
		}
	} else{
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
			EEPROM_Write(SETTINGS_PAGE+prof+1, SETTINGS_OFFSET, (void*)&zero_array, MODE_16_BIT, sizeof(Profile) >> 1);
			boot_mode_init();
	}
}
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
			if (stage == 0){
					profile_num = ((int) symbol_chars[symbol]) - '0';
					stage+=1;
					lcd_clear();
					display_menu_options();
					menu_prompt("ISS:");
			}
				
			else{
					lcd_put_char(symbol_chars[symbol]);
					keypad_input_to_number(row, col, &setting_val);
			}
			
			break;
		
		case SYMBOL_POUND:
			switch (stage){
				case 1:
					if (setting_val >= MIN_INTER_SYMBOL_SPACING_MS &&
				  setting_val <= MAX_INTER_SYMBOL_SPACING_MS) {
					curr_profile.settings.inter_symbol_spacing = setting_val;
					}
					setting_val = 0;
					stage+=1;
					lcd_clear();
					display_menu_options();
					menu_prompt("SYMLEN:");
					break;
				case 2:
					if (setting_val >= MIN_SYMBOL_LENGTH_MS &&
							setting_val <= MAX_SYMBOL_LENGTH_MS) {
						curr_profile.settings.symbol_length = setting_val;
					}
					setting_val = 0;
					stage+=1;
					lcd_clear();
					display_menu_options();
					menu_prompt("QUALITY:");
					break;
				case 3:
					if ( setting_val>= MIN_LUT_LOGSIZE &&
				  setting_val <= MAX_LUT_LOGSIZE) {
						curr_profile.settings.lut_logsize = setting_val;
					}
					setting_val = 0;
					stage+=1;
					lcd_clear();
					display_menu_options();
					menu_prompt("PROFILE LEN:");
					break;
				case 4:
					if ( setting_val>= 1 &&
				  setting_val <= 32) {
						curr_profile.length = setting_val;
					}
					setting_val = 0;
					curr_profile.settings.checksum = SETTINGS_CHECKSUM(curr_profile.settings);
					lcd_clear();
					lcd_set_cursor(0,0);
					keypad_set_read_callback(set_characters);
					break;
		}
		
		case SYMBOL_STAR:
			setting_val = 0;
			clear_user_input();
			break;
	}
}


void set_characters(int row, int col){
	static int i=1;
	static int checksum = 0;
	if (i < curr_profile.length){
		lcd_put_char(symbol_chars[SYMBOL(row, col)]);
		curr_profile.profile_characters[i-1]=SYMBOL(row, col);
		checksum = checksum ^ SYMBOL(row, col);
	}
	else{
		curr_profile.profile_characters[i-1]=SYMBOL(row, col);
		checksum = checksum ^ SYMBOL(row, col);
		checksum = checksum ^ curr_profile.settings.checksum ^ curr_profile.length;
		EEPROM_Write(SETTINGS_PAGE+profile_num+1, SETTINGS_OFFSET, (void*)&curr_profile, MODE_16_BIT, sizeof(Profile) >> 1);
		boot_mode_init();
	}
}

int checksum_check(Profile profile){
	int i;
	uint16_t calc_checksum = profile.settings.inter_symbol_spacing ^ profile.settings.lut_logsize ^ profile.settings.symbol_length;
	calc_checksum = calc_checksum ^ profile.length;
	for (i = 0; i<profile.length; i++){
		calc_checksum = calc_checksum ^ profile.profile_characters[i];
	}
	
	return calc_checksum;
}