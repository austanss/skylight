#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../pmm/pmm.h"
#include "../paging/paging.h"

#define HEAP_VIRTUAL_BASE       0x7f0000000000
#define HEAP_BITMAP_VBASE       0x7efffffe0000
#define MINIMUM_ALLOCATION_SIZE 0x40
#define HEAP_BITMAP_PAGES       0x10
#define HEAP_BITMAP_SIZE        HEAP_BITMAP_PAGES * PAGING_PAGE_SIZE * 8

static bool malloc_started = false;
static void* heap_start;
static void* heap_end;

static uint8_t* heap_map = NULL;

typedef struct allocation malloc_node_t;
struct allocation {
    uint64_t        index;
    uint64_t        address;
    uint64_t        allocations;
    malloc_node_t*  next;
};

static malloc_node_t node_root = {
    0,
    0,
    0,
    NULL
};

static void heap_map_set(uint64_t index, bool value) {
    uint8_t bit = index % 8;
    index /= 8;
    uint8_t mask = 0x80u >> bit;
    uint8_t byte = heap_map[index];
    byte &= ~mask;
    byte |= ((mask && value) << 7) >> bit;
    heap_map[index] = byte;
}

static bool heap_map_get(uint64_t index) {
    uint8_t bit = index % 8;
    index /= 8;
    uint8_t byte = heap_map[index];
    return byte & (0x80 >> bit);
}

static void _mark_allocations(uint64_t index, size_t count) {
    for (uint64_t indexer = index; indexer < index + count; indexer++)
        heap_map_set(indexer, true);
}

static void _free_allocations(uint64_t index, size_t count) {
    for (uint64_t indexer = index; indexer < index + count; indexer++)
        heap_map_set(indexer, false);
}

static void malloc_init() {
    void* map_base = pmm_alloc_page();
    heap_map = (uint8_t *)paging_map_page((void *)HEAP_BITMAP_VBASE, (void *)((uintptr_t)map_base - PAGING_VIRTUAL_OFFSET), PAGING_FLAGS_KERNEL_PAGE);

    for (uint64_t i = 1; i < HEAP_BITMAP_PAGES; i++) {
        void* map_extra = pmm_alloc_page();
        paging_map_page((void *)((uintptr_t)heap_map + i * PAGING_PAGE_SIZE), (void *)((uintptr_t)map_extra - PAGING_VIRTUAL_OFFSET), PAGING_FLAGS_KERNEL_PAGE);
    }

    void* hpage = pmm_alloc_page();
    void* vbase = (void *)HEAP_VIRTUAL_BASE;
    heap_start = paging_map_page(vbase, (void *)((uintptr_t)hpage - PAGING_VIRTUAL_OFFSET), PAGING_FLAGS_KERNEL_PAGE);
    heap_end = (void *)((uintptr_t)vbase + PAGING_PAGE_SIZE);

    for (size_t i = 0; i < HEAP_BITMAP_PAGES; i++) {
        void* page = pmm_alloc_page();
        void* virt = (void *)heap_end;
        paging_map_page(virt, (void *)((uintptr_t)page - PAGING_VIRTUAL_OFFSET), PAGING_FLAGS_KERNEL_PAGE);
        heap_end = (void *)((uintptr_t)heap_end + PAGING_PAGE_SIZE);
    }

    memset(heap_map, 0x00, HEAP_BITMAP_PAGES * PAGING_PAGE_SIZE);

    malloc_started = true;
}

static void* _malloc_single_allocation() {
    for (uint64_t index = 0; index < HEAP_BITMAP_SIZE; index++) {
        if (!heap_map_get(index)) {
            heap_map_set(index, true);
            void* p = (void *)(index * MINIMUM_ALLOCATION_SIZE + HEAP_VIRTUAL_BASE);
            return p;
        }
    }
    return NULL;
}

static void _free_single_allocation(void* p) {
    uint64_t index = ((uint64_t)p - HEAP_VIRTUAL_BASE) / MINIMUM_ALLOCATION_SIZE;
    heap_map_set(index, false);
}

void* malloc(size_t size) {
    if (!malloc_started)
        malloc_init();

    if (!size)
        return NULL;

    size = size + (MINIMUM_ALLOCATION_SIZE - (size % MINIMUM_ALLOCATION_SIZE));

    size_t allocations = size / MINIMUM_ALLOCATION_SIZE;

    bool found = false;
    uint64_t index;
    for (index = 0; index < HEAP_BITMAP_SIZE; index++) {
        if (!heap_map_get(index)) {
            uint64_t seek;
            for (seek = index; seek < HEAP_BITMAP_SIZE; seek++)
                if (heap_map_get(index))
                    break;

            if (seek - index >= allocations) {
                found = true;
                break;
            }
        }
    }

    if (!found)
        return NULL;

    _mark_allocations(index, allocations);

    malloc_node_t* new;

    for (new = &node_root; !!new->next; new = new->next);

    new->next = (malloc_node_t *)_malloc_single_allocation();

    if (!new->next)
        return NULL;

    new = new->next;

    new->index = index;
    new->allocations = allocations;
    new->next = NULL;
    new->address = (index * MINIMUM_ALLOCATION_SIZE) + HEAP_VIRTUAL_BASE;

    return (void *)new->address;
}

void free(void* p) {
    uint64_t index = ((uint64_t)p - PAGING_VIRTUAL_OFFSET) / PAGING_PAGE_SIZE; 
    malloc_node_t* tbf = &node_root;

    for (tbf = &node_root; tbf->next->index != index && !!tbf->next; tbf = tbf->next);

    if (!tbf->next)
        return;

    malloc_node_t* ahead = tbf->next->next;

    uint64_t allocations = tbf->next->allocations;

    _free_single_allocation(tbf->next);

    tbf->next = ahead;

    _free_allocations(index, allocations);
}
