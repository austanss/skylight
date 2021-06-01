#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct {
    unsigned char   character;
    uint32_t        color;
} __attribute__((packed)) tty_buffer_entry_t;

extern size_t               tty_width;
extern size_t               tty_height;
extern size_t               tty_row;
extern size_t               tty_column;
extern tty_buffer_entry_t*  tty_buffer;

void tty_putc(char c);
void tty_puts(const char* data);
void tty_shift();
void tty_clear();
void tty_set_cursor(size_t column, size_t row);
    