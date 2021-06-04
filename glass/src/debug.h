#pragma once
#include "dev/uart/serial.h"

#ifndef NDEBUG
#define DEBUG_PRINT(x, y) serial_terminal()->put##y(x);
#endif
