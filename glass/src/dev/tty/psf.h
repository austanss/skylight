#pragma once
#include <stdint.h>

#define PSF_MAGIC 0x864ab572
 
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t header_size;
    uint32_t flags;
    uint32_t glyph_count;
    uint32_t bytes_per_glyph;
    uint32_t height;
    uint32_t width;
} psf_font_t;
