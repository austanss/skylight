#include <string.h>
#include "paging.h"
#include "../pmm/pmm.h"

extern paging_table_t* pml4;

extern uint8_t __load_base;
extern uint8_t __load_max;

void paging_reload(struct stivale2_struct_tag_memmap* map) {
    asm volatile ("mov %%cr3, %0" : "=a"(pml4));

    memset(pml4, 0x00, PAGING_PAGE_SIZE);

    

    for (int i = 0; i < map->entries; i++)
        if (map->memmap[i].type == STIVALE2_MMAP_ACPI_RECLAIMABLE ||
            map->memmap[i].type == STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE ||
            map->memmap[i].type == STIVALE2_MMAP_KERNEL_AND_MODULES ||
            map->memmap[i].type == STIVALE2_MMAP_USABLE ||
            map->memmap[i].type == STIVALE2_MMAP_FRAMEBUFFER)
                for (uint64_t t = map->memmap[i].base; t < map->memmap[i].base + map->memmap[i].length; t+=PAGING_PAGE_SIZE)
                    paging_map_page((void *)(t + PAGING_VIRTUAL_OFFSET), (void *)t, PAGING_FLAGS_KERNEL_PAGE);

    asm volatile ("mov %0, %%cr3" : : "a"(pml4));
}