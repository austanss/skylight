#include <string.h>
#include "stivale.h"
#include "dev/uart/serial.h"
#include "mm/paging/paging.h"

static
PAGING_PAGE_ALIGNED
uint8_t stack[PAGING_PAGE_SIZE];

extern void boot(struct stivale2_struct*);

static struct stivale2_header_tag_terminal terminal_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
        .next = 0
    },
    .flags = 0
};

static struct stivale2_header_tag_framebuffer framebuffer_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uintptr_t)&terminal_tag
    },
};

__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header boot_header = {
    .entry_point = (uintptr_t)&boot,
    .stack = (uintptr_t)&stack + sizeof(stack),
    .flags = 0x02,
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

struct stivale2_module* get_module(struct stivale2_struct* bctx, char* cmdline) {
    struct stivale2_struct_tag_modules* modules = (struct stivale2_struct_tag_modules *)get_tag(bctx, STIVALE2_STRUCT_TAG_MODULES_ID);

    if (strlen(cmdline) > 128)
        return NULL;

    for (size_t i = 0; i < modules->module_count; i++)
        if (!strcmp(cmdline, modules->modules[i].string))
            return &modules->modules[i];

    return NULL;
}
