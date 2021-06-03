#include "tty.h"
#define SSFN_IMPLEMENTATION
#include "./ssfn.h"
#include "boot/stivale.h"
#include <stdbool.h>
#include <string.h>
#include "dev/uart/serial.h"

size_t tty_width;
size_t tty_height;
static size_t tty_row;
static size_t tty_column;
static tty_buffer_entry_t* tty_buffer;

extern struct stivale2_struct* bootctx;

static ssfn_t ctx = { 0 };
static ssfn_buf_t buf;


static uint32_t _active_color = 0xFFFFFFFF;

static bool _initialized = false;

static void _initialize() {
    struct stivale2_struct_tag_framebuffer* fb = (struct stivale2_struct_tag_framebuffer *)get_tag(bootctx, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    struct stivale2_module* font = get_module(bootctx, "KTTY-SFFN-FONT");

    buf.ptr = (uint8_t *)fb->framebuffer_addr;
    buf.w = (int16_t)fb->framebuffer_width;
    buf.h = (int16_t)fb->framebuffer_height;
    buf.p = fb->framebuffer_pitch;
    buf.x = 04;
    buf.y = 10;
    buf.fg = _active_color;

    memset(buf.ptr, 000, buf.h * buf.p);

    ssfn_load(&ctx, (const void *)font->begin);
    ssfn_select(&ctx, SSFN_FAMILY_ANY, NULL, SSFN_STYLE_REGULAR, 16);

    _initialized = true;
    tty_puts("glass: initialized kernel tty");
}

void tty_puts(const char* s) {
    if (!_initialized)
        _initialize();

    int ret = 0;
    while((ret = ssfn_render(&ctx, &buf, s)) > 0)
        s += ret;
}

void tty_putc(char c) {
    if (!_initialized)
        _initialize();

    if (!c)
        return;
    
    if (c == ' ') {
        
    }

    ssfn_render(&ctx, &buf, (const char *)&c);
}
