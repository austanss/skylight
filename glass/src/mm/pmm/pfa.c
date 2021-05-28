#include "pmm.h"
#include "../paging/paging.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

extern bool     pfa_allowing_allocations;
extern uint8_t* allocation_map;
extern uint64_t fast_index;
extern uint64_t map_size;

typedef struct page_pool page_pool_t;
struct page_pool {
    uint64_t        index;
    uint64_t        pages;
    page_pool_t*    next;
};

static page_pool_t pool_root = {
    0,
    0,
    NULL
};

void pmm_map_set(uint64_t index, bool value);
uint8_t pmm_map_get(uint64_t index);
void pmm_reindex(void);

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
        pmm_lock_page((void *)((uint64_t)page + i * 0x1000));
}

void pmm_unlock_pages(void* page, size_t count) {
    if (!pfa_allowing_allocations)
        return;
        
    for (size_t i = 0; i < count; i++)
        pmm_unlock_page((void *)((uint64_t)page + i * 0x1000));
}

void* pmm_alloc_pool(size_t page_count) {

    bool found = false;
    uint64_t index;
    for (index = 0; index < map_size; index++) {
        if (!pmm_map_get(index)) {
            uint64_t seek;
            for (seek = index; seek < map_size; seek++)
                if (pmm_map_get(index))
                    break;

            if (seek - index >= page_count) {
                found = true;
                break;
            }
        }
    }

    if (!found)
        return NULL;
    
    page_pool_t* new = &pool_root;

    for (new = &pool_root; !!new->next; new = new->next);

    new->next = (page_pool_t *)malloc(sizeof(page_pool_t));
    new = new->next;

    new->index = index;
    new->pages = page_count;
    new->next = NULL;

    void* start = (void *)((index * PAGING_PAGE_SIZE) + PAGING_VIRTUAL_OFFSET);

    pmm_lock_pages(start, page_count);

    return start;
}

void pmm_free_pool(void* pool) {
    uint64_t index = ((uint64_t)pool - PAGING_VIRTUAL_OFFSET) / PAGING_PAGE_SIZE; 
    page_pool_t* new = &pool_root;

    for (new = &pool_root; new->next->index != index && !!new->next; new = new->next);

    if (!new->next)
        return;

    uint64_t pages = new->next->pages;

    free(new->next);

    new->next = NULL;

    pmm_unlock_pages(pool, pages);
}

void* pmm_realloc_pool(void* pool, size_t page_count) {
    void* new_pool = pmm_alloc_pool(page_count);
    memcpy(new_pool, pool, page_count * PAGING_PAGE_SIZE);
    pmm_free_pool(pool);
    return new_pool;
}
