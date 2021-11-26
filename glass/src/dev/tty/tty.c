#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "boot/stivale.h"
#include "dev/tty/psf.h"
#include "dev/uart/serial.h"

bool tty_initialized = false;

struct stivale2_struct_tag_framebuffer* fb;

void tty_render_glyph(size_t x, size_t y, char c) {

    if (!tty_initialized) {
        struct stivale2_module* font = get_module(bootctx, "TTY-PSF");
        psf_load((void *)font->begin, (size_t)(font->end - font->begin));
        tty_initialized = true;
        fb = get_tag(bootctx, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    }

    serial_terminal()->putc('\n');

    for (unsigned char i = 32; i >= 32; i++) {
        uint16_t selector = psf_translation_table[i];
        serial_terminal()->putc(i)->putc(' ')->putul(selector)->puts("  ");
        if ((i+1) % 16 == 0)
            serial_terminal()->putc('\n');
    }

    uint16_t selector = psf_translation_table[c];

    psf_glyph_t glyph = psf_glyphs[selector];

    for (size_t yy = 0; yy < 16; yy++) {
        for (size_t xx = 0; xx < 8; xx++) {
            serial_terminal()->putc((glyph.data[yy] >> xx) & 1 ? '1' : '0');
            ((uint32_t *)fb->framebuffer_addr)[(y * fb->framebuffer_pitch + x) + (yy * fb->framebuffer_pitch + xx)] = (glyph.data[yy] >> xx) & 1 ? 0xFFFFFFFF : 0x00000000;
        }
        serial_terminal()->putc('\n');
    }

}
