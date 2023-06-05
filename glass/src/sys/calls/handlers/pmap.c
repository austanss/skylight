#include <stdint.h>
#include <stddef.h>

#include "mm/pmm/pmm.h"
#include "mm/paging/paging.h"

#define PMAP_VIRT_DEFAULT   0x0000

uint64_t pmap(void* virt) {
    void* phys = pmm_alloc_page() - PAGING_VIRTUAL_OFFSET;

    if (virt == PMAP_VIRT_DEFAULT)
        virt = phys + PAGING_VIRTUAL_OFFSET;

    return (uint64_t)paging_map_page(virt, phys, PAGING_FLAGS_USER_PAGE);
}
