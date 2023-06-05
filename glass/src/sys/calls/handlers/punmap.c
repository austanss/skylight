#include <stdint.h>
#include <stddef.h>

#include "mm/pmm/pmm.h"
#include "mm/paging/paging.h"

void punmap(void* virt) {
    if (!virt)
        return;

    void* phys = paging_walk_page(virt);
    paging_unmap_page(virt);
    paging_map_page(phys + PAGING_VIRTUAL_OFFSET, phys, PAGING_FLAGS_KERNEL_PAGE);

    return;
}
