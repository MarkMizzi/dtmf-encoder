#ifndef TONE_H
#define TONE_H

typedef enum {
	SQUARE,
	RAMP,
	SINE
} wavetype;

void tone_init(void);
void tone_play_with_interrupt(unsigned col, unsigned row);

#endif // TONE_H

// *******************************ARM University Program Copyright © ARM Ltd 2014*************************************   
