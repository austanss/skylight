#pragma once
#include <stdint.h>
#include <stddef.h>
#include "boot/stivale.h"

extern uint64_t total_memory;
extern uint64_t free_memory;

void*   pmm_alloc_page(void);
void    pmm_free_page(void* page);
void*   pmm_alloc_pool(size_t page_count);
void*   pmm_realloc_pool(void* pool, size_t page_count);
void    pmm_free_pool(void* pool);
void    pmm_lock_page(void* page);
void    pmm_unlock_page(void* page);
void    pmm_lock_pages(void* page, size_t count);
void    pmm_unlock_pages(void* page, size_t count);

