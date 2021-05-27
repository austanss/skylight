#pragma once
#include <stdint.h>

typedef struct serial_terminal {
    struct serial_terminal* (*puts)(const char* s);
    struct serial_terminal* (*putc)(const char  c);
    struct serial_terminal* (*putul)(uint64_t ul);
    struct serial_terminal* (*putd)(int64_t d);
} serial_terminal_t;

serial_terminal_t* serial_terminal();
