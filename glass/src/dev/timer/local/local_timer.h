#pragma once
#include <stdint.h>

#define LOCAL_TIMER_DIVISOR_002 0x00
#define LOCAL_TIMER_DIVISOR_004 0x01
#define LOCAL_TIMER_DIVISOR_008 0x02
#define LOCAL_TIMER_DIVISOR_016 0x03
#define LOCAL_TIMER_DIVISOR_032 0x04
#define LOCAL_TIMER_DIVISOR_064 0x05
#define LOCAL_TIMER_DIVISOR_128 0x06
#define LOCAL_TIMER_DIVISOR_001 0x07

void local_timer_calibrate();
void local_timer_set_frequency(uint64_t hz);

void local_timer_set_handler(void (*handler));

uint64_t local_timer_get_tpms();
