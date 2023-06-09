#pragma once
#include <stdint.h>

void tty_start();
void tty_kill();

void tty_clear();
void tty_set_cursor(uint64_t x, uint64_t y);

void tty_write(const char* str);
void tty_putc(char c);
