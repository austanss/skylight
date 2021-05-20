#include <string.h>
#include "paging.h"
#include "../pmm/pmm.h"

paging_table_t* pml4;

void* paging_map_page(void* virt, void* phys, uint16_t flags) {
    paging_indexer_t indexer;
    paging_indexer_assign(&indexer, virt);
    paging_desc_t pde;

    pde = pml4->entries[indexer.pml4];
    paging_table_t* pdp;
    if (!paging_desc_get_flag(&pde, PAGING_FLAG_PRESENT)) {
        pdp = (paging_table_t *)pmm_alloc_page();
        memset(pdp, 0, PAGING_PAGE_SIZE);
        paging_desc_set_address(&pde, (uint64_t)pdp);
        paging_desc_set_flag(&pde, PAGING_FLAG_PRESENT, true);
        paging_desc_set_flags(&pde, flags);
        pml4->entries[indexer.pml4] = pde;
        paging_map_page((void *)pdp, (void *)pdp, PAGING_FLAGS_KERNEL_PAGE);
    } else
        pdp = (paging_table_t*)((uint64_t)paging_desc_get_address(&pde));
    
    
    pde = pdp->entries[indexer.pml3];
    paging_table_t* pd;
    if (!paging_desc_get_flag(&pde, PAGING_FLAG_PRESENT)) {
        pd = (paging_table_t *)pmm_alloc_page();
        memset(pd, 0, PAGING_PAGE_SIZE);
        paging_desc_set_address(&pde, (uint64_t)pd);
        paging_desc_set_flag(&pde, PAGING_FLAG_PRESENT, true);
        paging_desc_set_flags(&pde, flags);
        pdp->entries[indexer.pml3] = pde;
        paging_map_page((void *)pd, (void *)pd, PAGING_FLAGS_KERNEL_PAGE);
    } else
        pd = (paging_table_t*)((uint64_t)paging_desc_get_address(&pde));

    pde = pd->entries[indexer.pml2];
    paging_table_t* pt;
    if (!paging_desc_get_flag(&pde, PAGING_FLAG_PRESENT)) {
        pt = (paging_table_t *)pmm_alloc_page();
        memset(pt, 0, PAGING_PAGE_SIZE);
        paging_desc_set_address(&pde, (uint64_t)pt);
        paging_desc_set_flag(&pde, PAGING_FLAG_PRESENT, true);
        paging_desc_set_flags(&pde, flags);
        pd->entries[indexer.pml2] = pde;
        paging_map_page((void *)pt, (void *)pt, PAGING_FLAGS_KERNEL_PAGE);
    } else
        pt = (paging_table_t*)((uint64_t)paging_desc_get_address(&pde));


    pde = pt->entries[indexer.pml1];
    paging_desc_set_address(&pde, (uint64_t)phys);
    paging_desc_set_flag(&pde, PAGING_FLAG_PRESENT, true);
    paging_desc_set_flags(&pde, flags);
    pt->entries[indexer.pml1] = pde;
    return virt;
}

void* paging_walk_page(void* virt) {
    paging_indexer_t indexer;
    paging_indexer_assign(&indexer, virt);
    paging_desc_t pde;

    pde = pml4->entries[indexer.pml4];
    paging_table_t* pdp;
    
    if (!paging_desc_get_flag(&pde, PAGING_FLAG_PRESENT))
        return NULL;
        
    pdp = (paging_table_t *)((uint64_t)paging_desc_get_address(&pde));
    
    pde = pdp->entries[indexer.pml3];
    paging_table_t* pd;

    if (!paging_desc_get_flag(&pde, PAGING_FLAG_PRESENT))
        return NULL;
        
    pd = (paging_table_t *)((uint64_t)paging_desc_get_address(&pde));

    pde = pd->entries[indexer.pml2];
    paging_table_t* pt;

    if (!paging_desc_get_flag(&pde, PAGING_FLAG_PRESENT))
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
    
    if (!paging_desc_get_flag(&pde, PAGING_FLAG_PRESENT))
        return;
        
    pdp = (paging_table_t *)((uint64_t)paging_desc_get_address(&pde));
    
    pde = pdp->entries[indexer.pml3];
    paging_table_t* pd;

    if (!paging_desc_get_flag(&pde, PAGING_FLAG_PRESENT))
        return;
        
    pd = (paging_table_t *)((uint64_t)paging_desc_get_address(&pde));

    pde = pd->entries[indexer.pml2];
    paging_table_t* pt;

    if (!paging_desc_get_flag(&pde, PAGING_FLAG_PRESENT))
        return;

    pt = (paging_table_t *)((uint64_t)paging_desc_get_address(&pde));

    pde = pt->entries[indexer.pml1];
    pde = 0;
    pt->entries[indexer.pml1] = pde;
}


void* paging_edit_page(void* virt, uint16_t flags) {
    void* phys = paging_walk_page(virt);
    
    if (!phys)
        return NULL;

    paging_map_page(virt, phys, flags);

    return virt;
}

void* paging_remap_page(void* old, void* new) {
    paging_indexer_t indexer;
    paging_indexer_assign(&indexer, old);
    paging_desc_t old_pde;

    old_pde = pml4->entries[indexer.pml4];
    paging_table_t* pdp;
    if (!paging_desc_get_flag(&old_pde, PAGING_FLAG_PRESENT))
        return NULL;
    else
        pdp = (paging_table_t *)((uint64_t)paging_desc_get_address(&old_pde));
    
    
    old_pde = pdp->entries[indexer.pml3];
    paging_table_t* pd;
    if (!paging_desc_get_flag(&old_pde, PAGING_FLAG_PRESENT)) 
        return NULL;
    else
        pd = (paging_table_t *)((uint64_t)paging_desc_get_address(&old_pde));

    old_pde = pd->entries[indexer.pml2];
    paging_table_t* pt;
    if (!paging_desc_get_flag(&old_pde, PAGING_FLAG_PRESENT)) 
        return NULL;
    else
        pt = (paging_table_t *)((uint64_t)paging_desc_get_address(&old_pde));

    old_pde = pt->entries[indexer.pml1];
    
    paging_unmap_page(old);

    paging_map_page(new, (void *)paging_desc_get_address(&old_pde), old_pde &= 0x8000000000000fff);

    return new;
}