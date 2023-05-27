#pragma once
#include <stdint.h>
#include <stddef.h>
#include "boot/protocol.h"

extern uint64_t total_memory;
extern uint64_t free_memory;

static inline char* memtype_string(uint32_t type) {
    switch (type) {
        case MEMORY_MAP_FREE: return "free";
        case MEMORY_MAP_BUSY: return "used";
        case MEMORY_MAP_MMIO: return "mmio";
        case MEMORY_MAP_NOUSE: return "bad";
    }
    return "unidentifiable";
}

void*   pmm_alloc_page(void);
void    pmm_free_page(void* page);
void*   pmm_alloc_pool(size_t page_count);
void*   pmm_realloc_pool(void* pool, size_t page_count);
void    pmm_free_pool(void* pool);
void    pmm_lock_page(void* page);
void    pmm_unlock_page(void* page);
void    pmm_lock_pages(void* page, size_t count);
void    pmm_unlock_pages(void* page, size_t count);

