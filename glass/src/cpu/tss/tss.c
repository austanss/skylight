#include "tss.h"
#include "../gdt/gdt.h"
#include "mm/paging/paging.h"
#include "mm/pmm/pmm.h"
#include <string.h>

extern tss_t tss_descriptors[];
tss_t tss_descriptors[TSS_MAX_CPUS];

static uint8_t ist_index = 0;

uint8_t tss_add_stack(int num_cpu) {
    if (ist_index >= 7)
        return 1;

    void* stack = pmm_alloc_page();

    tss_descriptors[num_cpu].ist[ist_index] = (uint64_t)stack + PAGING_PAGE_SIZE;
    ist_index++;
    return ist_index;
}

void tss_install(int num_cpu) {
    uint64_t tss_base = (uint64_t)&tss_descriptors[num_cpu];
    memset((void *)tss_base, 0, sizeof(tss_t));
    
    tss_add_stack(num_cpu);
    tss_add_stack(num_cpu);

    tss_descriptors[num_cpu].rsp[0] = tss_descriptors[num_cpu].ist[0];

    tss_descriptors[num_cpu].io_map = sizeof(tss_t);

    tss_reload(gdt_install_tss(tss_base));
}

tss_t* tss_get(int num_cpu) {
    return &tss_descriptors[num_cpu];
}
