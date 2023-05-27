#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "boot/protocol.h"
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
            ((uint32_t *)framebuffer.frame_addr)[(y * framebuffer.frame_width + x) + (yy * framebuffer.frame_width + (7 - xx))] = (glyph.data[yy] >> xx) & 1 ? 0xFFFFFFFF : 0x00000000;
}

void tty_enable() {
    if (tty_enabled) {
        tty_clear();
        return;
    }

    boot_module_t* font = get_boot_module("font.psf");
    psf_load((void *)font->phys, font->size);
    
    tty_enabled = true;

    tty_clear();

    tty_puts("Skylight v0.3: Sunrise!\n\n\n\nDon't try to do anything, it probably won't work. \nThe system is still in progress...");
}

void tty_disable() {
    if (!tty_enabled)
        return;

    //tty_clear();
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
    memset((void *)framebuffer.frame_addr, 0x00, framebuffer.frame_height * framebuffer.frame_width * (framebuffer.frame_bpp / 8));
}

void tty_putc(char c) {
    if (!tty_enabled)
        return;

    switch (c) {
        case '\n':
            tty_y += 16;
            tty_x = 0;
            if (tty_x >= framebuffer.frame_width) {
                tty_x = 0;
                tty_y += 16;
            }
            return;

        case '\r':
            tty_x = 0;
            if (tty_x >= framebuffer.frame_width) {
                tty_x = 0;
                tty_y += 16;
            }
            return;

        case '\t':
            tty_x = ((tty_x / 4) + 1) * 4;
            if (tty_x >= framebuffer.frame_width) {
                tty_x = 0;
                tty_y += 16;
            }
            return;
    }

    tty_render_glyph(tty_x, tty_y, c);

    tty_x += 8;
    if (tty_x >= framebuffer.frame_width) {
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
