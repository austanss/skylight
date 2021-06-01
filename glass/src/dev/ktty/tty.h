#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct {
    char            character;
    uint32_t        color;
} __attribute__((packed)) tty_buffer_entry_t;

extern size_t               tty_width;
extern size_t               tty_height;

void tty_putc(char c);
void tty_puts(const char* s);
void tty_shift(void);
void tty_clear(void);
void tty_set_cursor(size_t column, size_t row);
