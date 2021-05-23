#include <string.h>
#include "stivale.h"
#include "drivers/uart/serial.h"
#include "mm/paging/paging.h"

static
PAGING_PAGE_ALIGNED
uint8_t stack[PAGING_PAGE_SIZE];

extern void boot();

struct stivale2_header_tag_terminal terminal_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
        .next = 0
    },
    .flags = 0
};

struct stivale2_header_tag_framebuffer framebuffer_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uintptr_t)&terminal_tag
    },
};

__attribute__((section(".stivale2hdr"), used))
struct stivale2_header boot_header = {
    .entry_point = (uintptr_t)&boot,
    .stack = (uintptr_t)&stack + sizeof(stack),
    .flags = 0b10,
    .tags = (uintptr_t)&framebuffer_tag
};

void* get_tag(struct stivale2_struct *bctx, uint64_t id) {
    struct stivale2_tag *current_tag = (struct stivale2_tag *)bctx->tags;
    for (;;) {
        if (!current_tag)
            return NULL;
 
        if (current_tag->identifier == id)
            return (void *)current_tag;
 
        current_tag = (struct stivale2_tag *)current_tag->next;
    }
}