#pragma once

void timer_enable(void (*timer_callback)(void), float timer_freq);
void timer_disable(void);
