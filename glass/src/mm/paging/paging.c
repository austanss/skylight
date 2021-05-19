#include <string.h>
#include "paging.h"
#include "../pmm/pmm.h"

paging_table_t* pml4;

void* paging_map_page(void* virt, void* phys, uint16_t flags, bool execute) {
    paging_indexer_t indexer;
    paging_indexer_assign(&indexer, virt);
    paging_desc_t pde;

    pde = pml4->entries[indexer.pml4];
    paging_table_t* pdp;
    if (!PAGING_FLAG(pde, PAGING_FLAG_PRESENT)) {
        pdp = (paging_table_t *)pmm_alloc_page();
        memset(pdp, 0, 0x1000);
        paging_desc_set_address(&pde, (uint64_t)pdp);
        PAGING_FLAG_SET(pde, PAGING_FLAG_PRESENT);
        pde.flags |= flags;
        pml4->entries[indexer.pml4] = pde;
        paging_map_page((void *)pdp, (void *)pdp, PAGING_FLAGS_KERNEL_PAGE, false);
    } else
        pdp = (paging_table_t*)((uint64_t)paging_desc_get_address(&pde));
    
    
    pde = pdp->entries[indexer.pml3];
    paging_table_t* pd;
    if (!PAGING_FLAG(pde, PAGING_FLAG_PRESENT)) {
        pd = (paging_table_t *)pmm_alloc_page();
        memset(pd, 0, 0x1000);
        paging_desc_set_address(&pde, (uint64_t)pd);
        PAGING_FLAG_SET(pde, PAGING_FLAG_PRESENT);
        pde.flags |= flags;
        pdp->entries[indexer.pml3] = pde;
        paging_map_page((void *)pd, (void *)pd, PAGING_FLAGS_KERNEL_PAGE, false);
    } else
        pd = (paging_table_t*)((uint64_t)paging_desc_get_address(&pde));

    pde = pd->entries[indexer.pml2];
    paging_table_t* pt;
    if (!PAGING_FLAG(pde, PAGING_FLAG_PRESENT)) {
        pt = (paging_table_t *)pmm_alloc_page();
        memset(pt, 0, 0x1000);
        paging_desc_set_address(&pde, (uint64_t)pt);
        PAGING_FLAG_SET(pde, PAGING_FLAG_PRESENT);
        pde.flags |= flags;
        pd->entries[indexer.pml2] = pde;
        paging_map_page((void *)pt, (void *)pt, PAGING_FLAGS_KERNEL_PAGE, false);
    } else
        pt = (paging_table_t*)((uint64_t)paging_desc_get_address(&pde));


    pde = pt->entries[indexer.pml1];
    paging_desc_set_address(&pde, (uint64_t)phys);
    PAGING_FLAG_SET(pde, PAGING_FLAG_PRESENT);
    pde.flags |= flags;
    pde.nx = !execute;
    pt->entries[indexer.pml1] = pde;
    return virt;
}

void* paging_walk_page(void* virt) {
    paging_indexer_t indexer;
    paging_indexer_assign(&indexer, virt);
    paging_desc_t pde;

    pde = pml4->entries[indexer.pml4];
    paging_table_t* pdp;
    
    if (!PAGING_FLAG(pde, PAGING_FLAG_PRESENT))
        return NULL;
        
    pdp = (paging_table_t *)((uint64_t)paging_desc_get_address(&pde));
    
    pde = pdp->entries[indexer.pml3];
    paging_table_t* pd;

    if (!PAGING_FLAG(pde, PAGING_FLAG_PRESENT))
        return NULL;
        
    pd = (paging_table_t *)((uint64_t)paging_desc_get_address(&pde));

    pde = pd->entries[indexer.pml2];
    paging_table_t* pt;

    if (!PAGING_FLAG(pde, PAGING_FLAG_PRESENT))
        return NULL;

    pt = (paging_table_t *)((uint64_t)paging_desc_get_address(&pde));

    pde = pt->entries[indexer.pml1];
    return paging_desc_get_address(&pde);
}

void paging_unmap_page(void* virt) {
    paging_indexer_t indexer;
    paging_indexer_assign(&indexer, virt);
    paging_desc_t pde;

    pde = pml4->entries[indexer.pml4];
    paging_table_t* pdp;
    
    if (!PAGING_FLAG(pde, PAGING_FLAG_PRESENT))
        return;
        
    pdp = (paging_table_t *)((uint64_t)paging_desc_get_address(&pde));
    
    pde = pdp->entries[indexer.pml3];
    paging_table_t* pd;

    if (!PAGING_FLAG(pde, PAGING_FLAG_PRESENT))
        return;
        
    pd = (paging_table_t *)((uint64_t)paging_desc_get_address(&pde));

    pde = pd->entries[indexer.pml2];
    paging_table_t* pt;

    if (!PAGING_FLAG(pde, PAGING_FLAG_PRESENT))
        return;

    pt = (paging_table_t *)((uint64_t)paging_desc_get_address(&pde));

    pde = pt->entries[indexer.pml1];
    pde.flags = 0;
    pde.address = 0;
    pde.nx = NULL;
    pt->entries[indexer.pml1] = pde;
}


void* paging_edit_page(void* virt, uint16_t flags, bool execute) {
    void* phys = paging_walk_page(virt);
    
    if (!phys)
        return NULL;

    paging_map_page(virt, phys, flags, execute);

    return virt;
}

void* paging_remap_page(void* old, void* new) {
    paging_indexer_t indexer;
    paging_indexer_assign(&indexer, old);
    paging_desc_t old_pde;

    old_pde = pml4->entries[indexer.pml4];
    paging_table_t* pdp;
    if (!PAGING_FLAG(old_pde, PAGING_FLAG_PRESENT))
        return NULL;
    else
        pdp = (paging_table_t *)((uint64_t)paging_desc_get_address(&old_pde));
    
    
    old_pde = pdp->entries[indexer.pml3];
    paging_table_t* pd;
    if (!PAGING_FLAG(old_pde, PAGING_FLAG_PRESENT)) 
        return NULL;
    else
        pd = (paging_table_t *)((uint64_t)paging_desc_get_address(&old_pde));

    old_pde = pd->entries[indexer.pml2];
    paging_table_t* pt;
    if (!PAGING_FLAG(old_pde, PAGING_FLAG_PRESENT)) 
        return NULL;
    else
        pt = (paging_table_t *)((uint64_t)paging_desc_get_address(&old_pde));

    old_pde = pt->entries[indexer.pml1];
    
    paging_unmap_page(old);

    paging_map_page(new, (void *)old_pde.address, old_pde.flags, !old_pde.nx);

    return new;
}