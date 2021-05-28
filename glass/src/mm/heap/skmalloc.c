#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../pmm/pmm.h"
#include "../paging/paging.h"

#define HEAP_VIRTUAL_BASE 0x7f0000000000

static bool malloc_started = false;
static void* heap_start;
static void* heap_end;

void malloc_extend(size_t pages);
void malloc_extend(size_t pages) {
    for (size_t i = 0; i < pages; i++) {
        void* page = pmm_alloc_page();
        void* virt = (void *)heap_end;
        paging_map_page(virt, (void *)((uintptr_t)page - PAGING_VIRTUAL_OFFSET), PAGING_FLAGS_KERNEL_PAGE);
        heap_end = (void *)((uintptr_t)heap_end + PAGING_PAGE_SIZE);
    }
}

void malloc_shrink(size_t pages);
void malloc_shrink(size_t pages) {
    for (size_t i = 0; i < pages; i++) {
        void* virt = (void *)((uintptr_t)heap_end - PAGING_PAGE_SIZE);
        void* page = paging_walk_page(virt);
        page = (void *)((uintptr_t)page + PAGING_VIRTUAL_OFFSET);
        paging_unmap_page(virt);
        pmm_free_page(page);
        heap_end = (void *)((uintptr_t)heap_end - PAGING_PAGE_SIZE);
    }
}

void malloc_init(void);
void malloc_init() {
    void* page = pmm_alloc_page();
    void* vbase = (void *)HEAP_VIRTUAL_BASE;
    heap_start = paging_map_page(vbase, (void *)((uintptr_t)page - PAGING_VIRTUAL_OFFSET), PAGING_FLAGS_KERNEL_PAGE);
    heap_end = (void *)((uintptr_t)vbase + PAGING_PAGE_SIZE);
    malloc_extend(0xff);

    malloc_started = true;
}

void* malloc(size_t size) {
    if (!malloc_started)
        malloc_init();

    if (!size)
        return NULL;

    if (size >= ((uintptr_t)heap_end - (uintptr_t)heap_start))
        malloc_extend((size - ((uintptr_t)heap_end - (uintptr_t)heap_start)) / PAGING_PAGE_SIZE);

    return NULL;
}
