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

    uint16_t selector = psf_translation_table[c];

    psf_glyph_t glyph = psf_glyphs[selector];

    for (size_t yy = 15; yy < 16; yy--)
        for (size_t xx = 7; xx < 8; xx--)
            ((uint32_t *)fb->framebuffer_addr)[(y * fb->framebuffer_width + x) + ((15 - yy) * fb->framebuffer_width + (7 - xx))] = (glyph.data[yy] >> xx) & 1 ? 0xFFFFFFFF : 0x00000000;

}
