#include "tty.h"
#include "boot/stivale.h"
#include <stdbool.h>
#include <string.h>

size_t tty_width;
size_t tty_height;
static size_t tty_row;
static size_t tty_column;
static tty_buffer_entry_t* tty_buffer;

extern struct stivale2_struct* bootctx;

static uint32_t _active_color = 0xFFFFFF;

static bool _initialized = false;

static void _initialize() {
    struct stivale2_struct_tag_framebuffer* fb = (struct stivale2_struct_tag_framebuffer *)get_tag(bootctx, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    tty_height = fb->framebuffer_height / 8;
    tty_width = fb->framebuffer_width / 8;
}

void tty_puts(const char* s) {
    if (!_initialized)
        _initialize();

    size_t n = strlen(s);
    for (size_t i = 0; i < n; i++) {
        tty_putc(s[i]);
    }
}

void tty_putc(char c) {
    if (!_initialized)
        _initialize();

    tty_buffer_entry_t* entry = &tty_buffer[tty_row * tty_width + tty_column];
    entry->character = c;
    entry->color = _active_color;
}
