#include "gdt.h"
#include "mm/paging/paging.h"

PAGING_PAGE_ALIGNED
gdt_desc_t gdt[GDT_MAX_DESCRIPTORS];

gdtr_t gdtr;

uint8_t gindex;

void gdt_assemble() {
    gdtr.limit = (sizeof(gdt_desc_t) * GDT_MAX_DESCRIPTORS) - 1;
    gdtr.base = (uintptr_t)&gdt[0];

    gdt_add_descriptor(0, 0, 0, 0);
    gdt_add_descriptor(0, 0, GDT_BASIC_DESCRIPTOR | GDT_DESCRIPTOR_EXECUTABLE, GDT_BASIC_GRANULARITY);
    gdt_add_descriptor(0, 0, GDT_BASIC_DESCRIPTOR, GDT_BASIC_GRANULARITY);
    gdt_add_descriptor(0, 0, GDT_BASIC_DESCRIPTOR | GDT_DESCRIPTOR_DPL, GDT_BASIC_GRANULARITY);
    gdt_add_descriptor(0, 0, GDT_BASIC_DESCRIPTOR | GDT_DESCRIPTOR_DPL | GDT_DESCRIPTOR_EXECUTABLE, GDT_BASIC_GRANULARITY);

    gdt_reload(&gdtr, GDT_OFFSET_KERNEL_CODE, GDT_OFFSET_KERNEL_DATA);
}

void gdt_add_descriptor(uint64_t base, uint64_t limit, uint8_t access, uint8_t granularity) {
    if (gindex >= GDT_MAX_DESCRIPTORS) 
        return;

    gdt[gindex].base_low = base & 0xFFFF;
    gdt[gindex].base_mid = (base >> 16) & 0xFF;
    gdt[gindex].base_high = (base >> 24) & 0xFF;

    gdt[gindex].limit = limit & 0xFFFF;

    gdt[gindex].flags = access;
    gdt[gindex].granularity = granularity;

    gindex++;
}