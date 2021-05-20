#include <string.h>
#include "paging.h"
#include "../pmm/pmm.h"

extern paging_table_t* pml4;

extern uint8_t __code_base;
extern uint8_t __code_end;

void paging_reload(struct stivale2_struct_tag_framebuffer* fb_info) {
    pml4 = pmm_alloc_page();
    memset(pml4, 0x00, PAGING_PAGE_SIZE);

    for (unsigned long t = 0x0; t < total_memory; t+=PAGING_PAGE_SIZE)
        paging_map_page((void *)t, (void *)t, PAGING_FLAGS_KERNEL_PAGE);
    
    for (unsigned long t = 0x0; t < 0x80000000; t+=PAGING_PAGE_SIZE)
        paging_map_page((void *)t + 0xffffffff80000000, (void *)t, PAGING_FLAGS_KERNEL_PAGE);

    const unsigned long fb_size = (fb_info->framebuffer_bpp / 8) * fb_info->framebuffer_height * fb_info->framebuffer_width;

    for (unsigned long t = fb_info->framebuffer_addr; t < fb_info->framebuffer_addr + fb_size; t+=PAGING_PAGE_SIZE)
        paging_map_page((void *)t, (void *)t, PAGING_FLAGS_KERNEL_PAGE);

    asm volatile ("mov %0, %%cr3" : : "a"(pml4));
}