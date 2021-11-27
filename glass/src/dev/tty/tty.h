#pragma once
#include <stdint.h>
#include <stddef.h>

void tty_enable();
void tty_disable();

void tty_clear();

void tty_set_pos(size_t x, size_t y);

void tty_putc(char c);
void tty_puts(char* s);
