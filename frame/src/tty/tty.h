#pragma once
#include <stdint.h>

#define TTY_COLOR_BLACK 0
#define TTY_COLOR_BLUE 1
#define TTY_COLOR_GREEN 2
#define TTY_COLOR_CYAN 3
#define TTY_COLOR_RED 4
#define TTY_COLOR_MAGENTA 5
#define TTY_COLOR_BROWN 6
#define TTY_COLOR_LIGHT_GREY 7
#define TTY_COLOR_DARK_GREY 8
#define TTY_COLOR_LIGHT_BLUE 9
#define TTY_COLOR_LIGHT_GREEN 10
#define TTY_COLOR_LIGHT_CYAN 11
#define TTY_COLOR_LIGHT_RED 12
#define TTY_COLOR_LIGHT_MAGENTA 13
#define TTY_COLOR_LIGHT_BROWN 14
#define TTY_COLOR_WHITE 15

void tty_start();
void tty_kill();

void tty_clear();
void tty_set_cursor(uint64_t x, uint64_t y);

void tty_write(const char* str);
void tty_putc(char c, uint8_t color_code);
