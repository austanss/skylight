#pragma once
#include <stdint.h>
#include <stddef.h>
#include "stivale.h"

static uint64_t free_memory;
static uint64_t used_memory;

void    pmm_start(stivale2_struct_tag_memmap* memory_map);

void*   pmm_alloc_page();
void    pmm_free_page(void* page);
void*   pmm_alloc_pool(size_t page_count);
void    pmm_free_pool(void* pool);
void    pmm_lock_page(void* page);
void    pmm_unlock_page(void* page);