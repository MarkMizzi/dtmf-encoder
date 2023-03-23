#pragma once

void timer_enable(void (*timer_callback)(void), unsigned timer_freq);
void timer_disable(void);
