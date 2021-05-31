#include "pmm.h"
#include "../paging/paging.h"
#include "dev/uart/serial.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

static char* memtype_string(uint32_t type) {
    switch (type) {
        case STIVALE2_MMAP_USABLE: return "free";
        case STIVALE2_MMAP_BAD_MEMORY: return "bad";
        case STIVALE2_MMAP_ACPI_RECLAIMABLE: return "used";
        case STIVALE2_MMAP_RESERVED: return "used";
        case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE: return "used";
        case STIVALE2_MMAP_ACPI_NVS: return "dangerous";
        case STIVALE2_MMAP_FRAMEBUFFER: return "framebuffer";
        case STIVALE2_MMAP_KERNEL_AND_MODULES: return "kernel";
    }
    return "unidentifiable";
}

extern bool pfa_allowing_allocations;
bool pfa_allowing_allocations = false;
extern uint8_t* allocation_map;
uint8_t* allocation_map = NULL;
extern uint64_t map_size;
uint64_t map_size = 0;
extern uint64_t fast_index;
uint64_t fast_index = 0;
uint64_t total_memory = 0;
uint64_t free_memory = 0;

void pmm_map_set(uint64_t index, bool value);
void pmm_map_set(uint64_t index, bool value) {
    uint8_t bit = index % 8;
    index /= 8;
    uint8_t mask = 0x80u >> bit;
    uint8_t byte = allocation_map[index];
    byte &= ~mask;
    byte |= ((mask && value) << 7) >> bit;
    allocation_map[index] = byte;
}

uint8_t pmm_map_get(uint64_t index);
uint8_t pmm_map_get(uint64_t index) {
    uint8_t bit = index % 8;
    index /= 8;
    uint8_t byte = allocation_map[index];
    return byte & (0x80 >> bit);
}

void pmm_reindex(void);
void pmm_reindex() {
    for (uint64_t index = 0; index < map_size * 8; index++) {
        if (!pmm_map_get(index)) {
            fast_index = index;
            return;
        }
    }
    fast_index = 0;
    return;
}

void pmm_start(struct stivale2_struct_tag_memmap* memory_map_info);
void pmm_start(struct stivale2_struct_tag_memmap* memory_map_info) {
    if (pfa_allowing_allocations)
        return;

    total_memory = 0;

    for (uint64_t i = 0; i < memory_map_info->entries; i++)
        if (memory_map_info->memmap[i].type == STIVALE2_MMAP_USABLE ||
            memory_map_info->memmap[i].type == STIVALE2_MMAP_KERNEL_AND_MODULES ||
            memory_map_info->memmap[i].type == STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE ||
            memory_map_info->memmap[i].type == STIVALE2_MMAP_ACPI_RECLAIMABLE ||
            memory_map_info->memmap[i].type == STIVALE2_MMAP_ACPI_NVS)
                total_memory = memory_map_info->memmap[i].base + memory_map_info->memmap[i].length;

    map_size = total_memory / PAGING_PAGE_SIZE / 8;

    for (uint64_t i = 0; i < memory_map_info->entries; i++) {
        if (memory_map_info->memmap[i].type == 1) {
            if (memory_map_info->memmap[i].length >= map_size && memory_map_info->memmap[i].base >= 0x100000) {
                allocation_map = (uint8_t *)memory_map_info->memmap[i].base + PAGING_VIRTUAL_OFFSET;
                break;
            }
        }
    }

    memset(allocation_map, 0xFF, (size_t)map_size);

    pfa_allowing_allocations = true;

    free_memory = 0;

    serial_terminal()->puts("\n\nmemory map:\n\n");
    for (uint64_t i = 0; i < memory_map_info->entries; i++) {
        if (memory_map_info->memmap[i].type == 1)
            pmm_unlock_pages((void *)(memory_map_info->memmap[i].base + PAGING_VIRTUAL_OFFSET), memory_map_info->memmap[i].length / PAGING_PAGE_SIZE);

        serial_terminal()->puts("base: ")->putul(memory_map_info->memmap[i].base)->putc(',');
        serial_terminal()->puts(" type: ")->puts(memtype_string(memory_map_info->memmap[i].type))->putc(',');
        serial_terminal()->puts(" pages: ")->putd(memory_map_info->memmap[i].length / PAGING_PAGE_SIZE)->putc('\n');
    }

    pmm_lock_pages((void *)(0x0 + PAGING_VIRTUAL_OFFSET), 0x100000 / PAGING_PAGE_SIZE);

    pmm_lock_pages(allocation_map, map_size / PAGING_PAGE_SIZE);

    pmm_reindex();
}
