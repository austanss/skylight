#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include "dev/tty/psf.h"

#define UNICODE_SS      0x00FE
#define UNICODE_TERM    0x00FF

psf_glyph_t* psf_glyphs;
uint16_t* psf_translation_table;

void psf_load(void* psf, size_t bytes) {
    psf_font_t* font = (psf_font_t *)psf;
    void* end = psf + bytes;

    if (font->magic != PSF_MAGIC || !font->flags) {
        psf_translation_table = NULL;
        return;
    }

    uint16_t current_glyph = 0;

    unsigned char* unicode_table = (unsigned char *)(psf + font->header_size + font->bytes_per_glyph * font->glyph_count);
    
    psf_glyphs = (psf_glyph_t *)(psf + font->header_size);
    psf_translation_table = (uint16_t *)calloc(USHRT_MAX, 2);

    while (unicode_table > (unsigned char *)end) {
        uint16_t unicode = (uint16_t)(unicode_table[0]);

        if (unicode == 0xFF) {
            current_glyph++;
            unicode_table++;
            continue;
        } else if (unicode & 128) {
            if ((unicode & 32) == 0) {
                unicode = ((unicode_table[0] & 0x1F)<<6)+(unicode_table[1] & 0x3F);
                unicode_table++;
            } else if ((unicode & 16) == 0) {
                unicode = ((((unicode_table[0] & 0xF)<<6)+(unicode_table[1] & 0x3F))<<6)+(unicode_table[2] & 0x3F);
                unicode_table += 2;
            } else if ((unicode & 8) == 0) {
                unicode = ((((((unicode_table[0] & 0x7)<<6)+(unicode_table[1] & 0x3F))<<6)+(unicode_table[2] & 0x3F))<<6)+(unicode_table[3] & 0x3F);
                unicode_table += 3;
            } else
                unicode = 0;
        }

        psf_translation_table[unicode] = current_glyph;
        unicode_table++;
    }
}
