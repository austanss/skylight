#include "cpu/interrupts/idt.h"
#include "cpu/gdt/gdt.h"
#include "cpu/tss/tss.h"
#include "mm/paging/paging.h"

PAGING_PAGE_ALIGNED
idt_desc_t __idt[IDT_MAX_DESCRIPTORS];

static idtr_t idtr;

static bool vectors[IDT_MAX_DESCRIPTORS];

uint64_t __routine_handlers[IDT_MAX_DESCRIPTORS];

extern uint64_t isr_stub_table[];

void idt_install_irq_handler(uint8_t irq, void* handler) {
    __routine_handlers[irq] = (uint64_t)handler;
    idt_set_descriptor(irq, isr_stub_table[irq], IDT_DESCRIPTOR_EXTERNAL, TSS_IST_ROUTINE);
}

void idt_set_descriptor(uint8_t vector, uintptr_t isr, uint8_t flags, uint8_t ist) {
    idt_desc_t* descriptor = &__idt[vector];

    descriptor->base_low       = isr & 0xFFFF;
    descriptor->cs             = GDT_OFFSET_KERNEL_CODE;
    descriptor->ist            = ist;
    descriptor->attributes     = flags;
    descriptor->base_mid       = (isr >> 16) & 0xFFFF;
    descriptor->base_high      = (isr >> 32) & 0xFFFFFFFF;
    descriptor->rsv0           = 0;
}

void idt_assemble() {
    idtr.base = (uintptr_t)&__idt[0];
    idtr.limit = (uint16_t)sizeof(idt_desc_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < IDT_CPU_EXCEPTION_COUNT; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], IDT_DESCRIPTOR_EXCEPTION, TSS_IST_EXCEPTION);
        vectors[vector] = true;
    }

    idt_reload(&idtr);
}

uint8_t idt_allocate_vector() {
    for (unsigned int i = 0; i < IDT_MAX_DESCRIPTORS; i++) {
        if (!vectors[i]) {
            vectors[i] = true;
            return (uint8_t)i;
        }
    }

    return 0;
}

void idt_free_vector(uint8_t vector) {
    idt_set_descriptor(vector, 0, 0, 0);
    __routine_handlers[vector] = 0;
    vectors[vector] = false;
}
