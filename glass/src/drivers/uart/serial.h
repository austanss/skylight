#pragma once

typedef struct serial_terminal {
    struct serial_terminal* (*puts)(const char* s);
    struct serial_terminal* (*putc)(const char  c);
} serial_terminal_t;

serial_terminal_t* serial_terminal();