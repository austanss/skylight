#include "pmm.h"
#include "../paging/paging.h"
#include <stdbool.h>

extern bool     pfa_allowing_allocations;
extern uint8_t* allocation_map;
extern uint64_t fast_index;

void pmm_map_set(uint64_t index, bool value);
uint8_t pmm_map_get(uint64_t index);
void pmm_reindex();

void* pmm_alloc_page() {
    if (!pfa_allowing_allocations)
        return NULL;

    if (!free_memory)
        return NULL;

    if (pmm_map_get(fast_index))
        pmm_reindex();

    if (!fast_index)
        return NULL;

    void* page = (void *)(fast_index * PAGING_PAGE_SIZE + PAGING_VIRTUAL_OFFSET);

    pmm_lock_page(page);

    return page;
}

void pmm_free_page(void* page) {
    if (!pfa_allowing_allocations)
        return;

    pmm_unlock_page(page);
}

void pmm_lock_page(void* page) {
    if (!pfa_allowing_allocations)
        return;
        
    uint64_t address = (uint64_t)page - PAGING_VIRTUAL_OFFSET;

    if (address > total_memory)
        return;

    uint64_t index = address / PAGING_PAGE_SIZE;
    pmm_map_set(index, true);

    free_memory -= PAGING_PAGE_SIZE;
}

void pmm_unlock_page(void* page) {
    if (!pfa_allowing_allocations)
        return;
        
    uint64_t address = (uint64_t)page - PAGING_VIRTUAL_OFFSET;

    if (address > total_memory)
        return;

    uint64_t index = address / PAGING_PAGE_SIZE;
    pmm_map_set(index, false);

    free_memory += PAGING_PAGE_SIZE;
}

void pmm_lock_pages(void* page, size_t count) {
    if (!pfa_allowing_allocations)
        return;
        
    for (size_t i = 0; i < count; i++)
        pmm_lock_page(page + i * 0x1000);
}

void pmm_unlock_pages(void* page, size_t count) {
    if (!pfa_allowing_allocations)
        return;
        
    for (size_t i = 0; i < count; i++)
        pmm_unlock_page(page + i * 0x1000);
}