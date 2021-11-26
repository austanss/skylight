#pragma once
#include <stdint.h>

#define PSF_MAGIC 0x864ab572

#define PSF2_SEPARATOR      0xFF
#define PSF2_START_SEQUENCE 0xFE
 
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t header_size;
    uint32_t flags;
    uint32_t glyph_count;
    uint32_t bytes_per_glyph;
    uint32_t height, width;
} psf_font_t;

typedef struct {
    uint8_t data[16];
} psf_glyph_t;

void psf_load(void* psf, size_t bytes);

extern psf_glyph_t* psf_glyphs;
extern uint16_t* psf_translation_table;
