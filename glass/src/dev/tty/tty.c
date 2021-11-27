#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "boot/stivale.h"
#include "dev/tty/psf.h"
#include "dev/tty/tty.h"

bool tty_enabled = false;

struct stivale2_struct_tag_framebuffer* fb;

static size_t tty_x;
static size_t tty_y;

void tty_render_glyph(size_t x, size_t y, char c) {
    if (!tty_enabled)
        tty_enable();

    uint16_t selector = psf_translation_table[c];

    psf_glyph_t glyph = psf_glyphs[selector];

    for (size_t yy = 15; yy < 16; yy--)
        for (size_t xx = 7; xx < 8; xx--)
            ((uint32_t *)fb->framebuffer_addr)[(y * fb->framebuffer_width + x) + (yy * fb->framebuffer_width + (7 - xx))] = (glyph.data[yy] >> xx) & 1 ? 0xFFFFFFFF : 0x00000000;
}

void tty_enable() {
    if (tty_enabled) {
        tty_clear();
        return;
    }

    struct stivale2_module* font = get_module(bootctx, "TTY-PSF");
    psf_load((void *)font->begin, (size_t)(font->end - font->begin));
    fb = get_tag(bootctx, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    
    tty_enabled = true;

    tty_clear();
}

void tty_disable() {
    if (!tty_enabled)
        return;

    tty_clear();
    tty_enabled = false;
    free(psf_translation_table);
}

void tty_set_pos(size_t x, size_t y) {
    tty_x = x;
    tty_y = y;
}

void tty_clear() {
    if (!tty_enabled)
        return;

    tty_set_pos(0, 0);
    memset((void *)fb->framebuffer_addr, 0x00, fb->framebuffer_height * fb->framebuffer_width * (fb->framebuffer_bpp / 8));
}

void tty_putc(char c) {
    if (!tty_enabled)
        return;

    tty_render_glyph(tty_x, tty_y, c);

    switch (c) {
        case '\n':
            tty_y += 16;
            tty_x = 0;
            break;

        case '\r':
            tty_x = 0;
            break;

        case '\t':
            tty_x = ((tty_x / 4) + 1) * 4;
            break;
    }

    tty_x += 8;
    if (tty_x >= fb->framebuffer_width) {
        tty_x = 0;
        tty_y += 16;
    }
}

void tty_puts(char* s) {
    if (!tty_enabled)
        return;

    size_t len = strlen(s);
    for (size_t i = 0; i < len; i++) {
        tty_putc(s[i]);
    }
}
