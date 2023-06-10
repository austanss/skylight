#pragma once
#include <stdint.h>

#define PS2_KBD_COMMAND_PORT 0x64
#define PS2_KBD_STATUS_REG 0x64
#define PS2_KBD_DATA_PORT 0x60

void hid_enable_keyboard_interrupts();
