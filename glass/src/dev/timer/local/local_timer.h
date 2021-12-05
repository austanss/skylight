#pragma once
#include <stdint.h>

void local_timer_calibrate();
void local_timer_set_frequency(uint64_t hz);

void local_timer_set_handler(void (*handler));

uint64_t local_timer_get_tpms();
