#include "idt.h"
#include "../gdt/gdt.h"
#include "mm/paging/paging.h"

PAGING_PAGE_ALIGNED
idt_desc_t idt[IDT_MAX_DESCRIPTORS];

idtr_t idtr;

bool vectors[IDT_MAX_DESCRIPTORS];

extern uint64_t isr_stub_table[];

void idt_set_descriptor(uint8_t vector, uintptr_t isr, uint8_t flags, uint8_t ist) {
    idt_desc_t* descriptor = &idt[vector];

    descriptor->base_low       = isr & 0xFFFF;
    descriptor->cs             = GDT_OFFSET_KERNEL_CODE;
    descriptor->ist            = ist;
    descriptor->attributes     = flags;
    descriptor->base_mid       = (isr >> 16) & 0xFFFF;
    descriptor->base_high      = (isr >> 32) & 0xFFFFFFFF;
    descriptor->rsv0           = 0;
}

void idt_assemble() {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_desc_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < IDT_CPU_EXCEPTION_COUNT; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], IDT_DESCRIPTOR_EXCEPTION, 1);
        vectors[vector] = true;
    }

    idt_reload(&idtr);
}

uint8_t idt_allocate_vector() {
    for (unsigned int i = 0; i < IDT_MAX_DESCRIPTORS; i++) {
        if (!i) {
            vectors[i] = true;
            return (uint8_t)i;
        }
    }

    return 0;
}

void idt_free_vector(uint8_t vector) {
    idt_set_descriptor(vector, 0, 0, 0);
    vectors[vector] = false;
}
