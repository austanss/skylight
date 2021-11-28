#include <stdint.h>
#include <stddef.h>

#include "mm/pmm/pmm.h"
#include "boot/stivale.h"

uint64_t total_ram() {
    return total_memory;
}

uint64_t free_ram() {
    return free_memory;
}

static struct stivale2_struct_tag_framebuffer* fb = NULL;

uint64_t display_width() {
    if (!fb)
        fb = (struct stivale2_struct_tag_framebuffer *)get_tag(bootctx, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    
    return fb->framebuffer_width;
}

uint64_t display_height() {
    if (!fb)
        fb = (struct stivale2_struct_tag_framebuffer *)get_tag(bootctx, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    
    return fb->framebuffer_height;
}

uint64_t display_bpp() {
    if (!fb)
        fb = (struct stivale2_struct_tag_framebuffer *)get_tag(bootctx, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    
    return fb->framebuffer_bpp;
}

uint64_t (*subfunction[])() = {
    total_ram,
    free_ram,
    display_width,
    display_height,
    display_bpp
};

uint64_t rdinfo(uint64_t field) {
    return subfunction[field]();
}
