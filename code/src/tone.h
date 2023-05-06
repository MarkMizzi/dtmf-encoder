#ifndef TONE_H
#define TONE_H

#define SYMBOL_LENGTH_MS 600U
#define INTERSYMBOL_SPACING_MS 400U

void tone_init(void);
void tone_play_or_enqueue(int row, int col);

#endif // TONE_H

// *******************************ARM University Program Copyright © ARM Ltd 2014*************************************   
