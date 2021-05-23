#include <string.h>
#include "paging.h"
#include "../pmm/pmm.h"

extern paging_table_t* pml4;

extern uint8_t __load_base;
extern uint8_t __load_max;

void paging_reload(struct stivale2_struct_tag_memmap* map) {
    pml4 = (paging_table_t *)pmm_alloc_page();

    memset(pml4, 0x00, PAGING_PAGE_SIZE);

    for (uint64_t i = 0; i < map->entries; i++)
        if (map->memmap[i].type == STIVALE2_MMAP_ACPI_RECLAIMABLE ||
            map->memmap[i].type == STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE ||
            map->memmap[i].type == STIVALE2_MMAP_KERNEL_AND_MODULES ||
            map->memmap[i].type == STIVALE2_MMAP_USABLE ||
            map->memmap[i].type == STIVALE2_MMAP_FRAMEBUFFER)
                for (uint64_t t = map->memmap[i].base; t < map->memmap[i].base + map->memmap[i].length; t+=PAGING_PAGE_SIZE)
                    paging_map_page((void *)(t + PAGING_VIRTUAL_OFFSET), (void *)t, PAGING_FLAGS_KERNEL_PAGE);

    for (uint64_t t = (uint64_t)&__load_base; t < (uint64_t)&__load_max; t+=PAGING_PAGE_SIZE)
        paging_map_page((void *)t, (void *)(t - PAGING_KERNEL_OFFSET), PAGING_FLAGS_KERNEL_PAGE);

    asm volatile ("mov %0, %%cr3" : : "a"(pml4));
}