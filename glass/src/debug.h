#pragma once
#include "drivers/uart/serial.h"

#ifdef __DEBUG__
#define DEBUG_PRINT(x, y) serial_terminal()->put##y(x);
#endif